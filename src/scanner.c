#include "tree_sitter/parser.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <wctype.h>

#ifndef TREE_SITTER_LANGUAGE
#define TREE_SITTER_LANGUAGE red
#endif
#define tree_sitter___external_scanner(language, symbol)                       \
  tree_sitter_##language##_external_scanner_##symbol
#define tree_sitter__external_scanner(language, symbol)                        \
  tree_sitter___external_scanner(language, symbol)
#define tree_sitter_external_scanner(symbol)                                   \
  tree_sitter__external_scanner(TREE_SITTER_LANGUAGE, symbol)

#if defined(__wasi__) || defined(__EMSCRIPTEN__)
#else
#endif

#ifdef DEBUG
#include <stdio.h>
#define trace(string) printf(string)
#define tracef(format, ...) printf(format, __VA_ARGS__)
#else
#define trace(string)
#define tracef(format, ...)
#endif

struct Scanner {
  uint32_t delimiter_length;
};

#if __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(struct Scanner) <= TREE_SITTER_SERIALIZATION_BUFFER_SIZE,
               "Scanner too large");
#endif

enum TokenType {
  INFIX_OP,
  RED_HEXA,
  RAW_STRING,
  MULTILINE_STRING,
  IPV6_ADDRESS,
  ERROR_SENTINEL,
};

#ifdef DEBUG
static const char *const symbol_names[] = {
    [INFIX_OP] = "$._infix_op",
    [RED_HEXA] = "$.hexa",
    [RAW_STRING] = "$.raw_string",
    [MULTILINE_STRING] = "$.multiline_string",
    [IPV6_ADDRESS] = "$.ipv6_address",
};
#endif

void tree_sitter_external_scanner(reset)(void *payload) {
  struct Scanner *scanner = payload;
  scanner->delimiter_length = 0;
}

void *tree_sitter_external_scanner(create)(void) {
  struct Scanner *scanner = malloc(sizeof(struct Scanner));
  tree_sitter_external_scanner(reset)(scanner);
  return scanner;
}

void tree_sitter_external_scanner(destroy)(void *payload) { free(payload); }

unsigned tree_sitter_external_scanner(serialize)(void *payload, char *buffer) {
  trace("serializing\n");
  *(struct Scanner *)buffer = *(struct Scanner *)payload;
  return sizeof(struct Scanner);
}

void tree_sitter_external_scanner(deserialize)(void *payload,
                                               const char *buffer,
                                               unsigned length) {
  tree_sitter_external_scanner(reset)(payload);
  if (length != sizeof(struct Scanner)) {
    return;
  }
  *(struct Scanner *)payload = *(struct Scanner *)buffer;
}

static void advance(TSLexer *lexer) { lexer->advance(lexer, false); }
static void skip(TSLexer *lexer) { lexer->advance(lexer, true); }

static void skip_spaces(TSLexer *lexer) {
  while (iswspace(lexer->lookahead) && !lexer->eof(lexer)) {
    skip(lexer);
  }
}

#ifdef DEBUG
static bool trace_valid_symbols(const bool *valid_symbols) {
  for (int i = INFIX_OP; i < ERROR_SENTINEL; i++) {
    printf("valid_symbols[%s]: %s\n", symbol_names[i],
           valid_symbols[i] ? "true" : "false");
  }
  return false;
}
#define trace_valid_symbols(...) trace_valid_symbols(valid_symbols)
#else
#define trace_valid_symbols(...)
#endif

#define S_OK 0
#define S_RETURN 1
#define S_CONTINUE 2

/// Scan the raw string: /(%+)\{.*?\}\1/
static int scan_raw_string(TSLexer *lexer, int start) {
  skip_spaces(lexer);

  // Step 1: count leading %
  int left = start;
  while (lexer->lookahead == '%') {
    advance(lexer);
    left++;
  }
  if (left == 0)
    return S_CONTINUE;

  // Step 2: require opening brace
  if (lexer->lookahead != '{')
    return S_RETURN;
  advance(lexer);

  for (int delimiter_index = -1;;) {
    // If we hit EOF, consider the content to terminate there.
    // This forms an incomplete raw_string, and models the code well.
    if (lexer->eof(lexer)) {
      lexer->mark_end(lexer);
      lexer->result_symbol = RAW_STRING;
      return S_OK;
    }

    if (delimiter_index >= 0) {
      if (delimiter_index == left) {
        lexer->mark_end(lexer);
        lexer->result_symbol = RAW_STRING;
        return S_OK;
      } else {
        if (lexer->lookahead == '%') {
          delimiter_index += 1;
        } else {
          delimiter_index = -1;
        }
      }
    }
    if (delimiter_index == -1 && lexer->lookahead == '}') {
      delimiter_index = 0;
    }
    advance(lexer);
  }
  return S_RETURN;
}

static bool scan_multiline_string(TSLexer *lexer) {
  for (int cnt = 1;;) {
    // If we hit EOF, consider the content to terminate there.
    // This forms an incomplete raw_string, and models the code well.
    if (lexer->eof(lexer)) {
      lexer->mark_end(lexer);
      lexer->result_symbol = MULTILINE_STRING;
      return true;
    }

    switch (lexer->lookahead) {
    case '{':
      cnt++;
      break;
    case '}':
      cnt--;
      if (cnt == 0) {
        lexer->mark_end(lexer);
        lexer->result_symbol = MULTILINE_STRING;
        return true;
      }
      break;
    case '^':
      advance(lexer);
      switch (lexer->lookahead) {
      case '^':
      case '{':
      case '}':
        break;
      default:
        continue;
      }
    default:
      break;
    }
    advance(lexer);
  }
  return false;
}

static int scan_infix_op(TSLexer *lexer) {
  if (!iswspace(lexer->lookahead))
    return S_CONTINUE;

  skip_spaces(lexer);

  bool is_percent = false;
  bool find = true;
  int32_t c = lexer->lookahead;

  switch (c) {
  case '=':
    advance(lexer);
    if (lexer->lookahead == '=')
      advance(lexer);
    break;
  case '<':
    advance(lexer);
    c = lexer->lookahead;
    if (c == '=' || c == '<' || c == '>')
      advance(lexer);
    break;
  case '>':
    advance(lexer);
    c = lexer->lookahead;
    if (c == '=') {
      advance(lexer);
    } else if (c == '>') {
      advance(lexer);
      if (lexer->lookahead == '>')
        advance(lexer);
    }
    break;
  case '/':
    advance(lexer);
    c = lexer->lookahead;
    if (c == '/') {
      advance(lexer);
    }
    break;
  case '+':
    advance(lexer);
    break;
  case '-':
    advance(lexer);
    break;
  case '*':
    advance(lexer);
    break;
  case '%':
    advance(lexer);
    is_percent = true;
    break;
  default:
    find = false;
  }
  if (find) {
    if (!iswspace(lexer->lookahead) || lexer->eof(lexer)) {
      if (is_percent && !lexer->eof(lexer)) {
        // check if it's a raw string
        return scan_raw_string(lexer, 1);
      }
      return S_RETURN;
    }
    lexer->mark_end(lexer);
    lexer->result_symbol = INFIX_OP;
    return S_OK;
  }
  return S_CONTINUE;
}

static bool is_hex_upper(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

static inline bool is_hex(int32_t c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

static inline bool is_digit(int32_t c) { return c >= '0' && c <= '9'; }

static bool scan_remaining_ipv4_octets(TSLexer *lexer) {
  for (int i = 0; i < 3; i++) {
    if (lexer->lookahead != '.')
      return false;
    lexer->advance(lexer, false); // consume '.'

    int value = 0;
    int digits = 0;

    if (!is_digit(lexer->lookahead))
      return false;

    while (is_digit(lexer->lookahead)) {
      value = value * 10 + (lexer->lookahead - '0');
      digits++;
      if (digits > 3 || value > 255)
        return false;
      lexer->advance(lexer, false);
    }
  }
  return true;
}

int scan_ipv6(TSLexer *lexer, int start) {
  int groups = 0;
  bool seen_double_colon = false;

  int32_t c = lexer->lookahead;
  if (start == 0) {
    if (!(is_hex(c) || c == ':'))
      return S_CONTINUE;

    // start with "::"
    if (c == ':') {
      lexer->advance(lexer, false);
      if (lexer->lookahead != ':')
        return S_RETURN;
      lexer->advance(lexer, false);
      seen_double_colon = true;
    }
  }

  while (true) {
    int hex_count = 0;
    bool decimal_only = true;
    int decimal_value = 0;
    int decimal_digits = 0;

    while (is_hex(lexer->lookahead)) {
      int32_t ch = lexer->lookahead;

      if (!is_digit(ch)) {
        decimal_only = false;
      } else if (decimal_only) {
        decimal_value = decimal_value * 10 + (ch - '0');
        decimal_digits++;
        if (decimal_digits > 3 || decimal_value > 255) {
          decimal_only = false;
        }
      }

      lexer->advance(lexer, false);
      hex_count++;
    }

    if (start > 0) {
      hex_count = start;
      start = 0;
    }
    if (hex_count == 0)
      break;
    if (hex_count > 4)
      return S_RETURN;

    groups++;

    if (lexer->lookahead == '.') {
      if (!decimal_only || decimal_digits == 0) {
        return S_RETURN;
      }

      if (!scan_remaining_ipv4_octets(lexer)) {
        return S_RETURN;
      }

      groups += 1;
      break;
    }

    if (lexer->lookahead != ':')
      break;

    lexer->advance(lexer, false);

    if (lexer->lookahead == ':') {
      if (seen_double_colon)
        return S_RETURN;
      seen_double_colon = true;
      lexer->advance(lexer, false);
      continue;
    }

    if (!is_hex(lexer->lookahead)) {
      return S_RETURN;
    }
  }

  if (seen_double_colon) {
    if (groups > 7)
      return S_RETURN;
  } else {
    if (groups != 8)
      return S_RETURN;
  }

  lexer->mark_end(lexer);
  lexer->result_symbol = IPV6_ADDRESS;
  return S_OK;
}

bool tree_sitter_external_scanner(scan)(void *payload, TSLexer *lexer,
                                        const bool *valid_symbols) {
  trace("==========\n");
  tracef("lookahead: %d\n", lexer->lookahead);
  trace_valid_symbols(valid_symbols);

  if (lexer->eof(lexer)) {
    return false;
  }

  if (valid_symbols[ERROR_SENTINEL]) {
    return false;
  }

  if (valid_symbols[INFIX_OP]) {
    switch (scan_infix_op(lexer)) {
    case S_OK:
      return true;
    case S_RETURN:
      return false;
    default:
      break;
    }
  }

  if (valid_symbols[RED_HEXA]) {
    skip_spaces(lexer);
    int count = 0;
    // 2 - 8 characters
    while (is_hex_upper(lexer->lookahead) && count < 8) {
      advance(lexer);
      count++;
    }
    if (count >= 2 && lexer->lookahead == 'h') {
      advance(lexer);
      int32_t c = lexer->lookahead;
      // check valid tail chars
      if (iswspace(c) || c == ']' || c == '[' || c == '{' || c == '"' ||
          c == '(' || c == ')' || c == '<' || lexer->eof(lexer)) {
        lexer->mark_end(lexer);
        lexer->result_symbol = RED_HEXA;
        return true;
      }
      return false;
    }

    if (count > 0) {
      if (lexer->lookahead == ':' && count <= 4) {
        return S_OK == scan_ipv6(lexer, count);
      }
      return false;
    }
  }

  if (valid_symbols[IPV6_ADDRESS]) {
    skip_spaces(lexer);
    switch (scan_ipv6(lexer, 0)) {
    case S_OK:
      return true;
    case S_RETURN:
      return false;
    default:
      break;
    }
  }

  if (valid_symbols[RAW_STRING]) {
    switch (scan_raw_string(lexer, 0)) {
    case S_OK:
      return true;
    case S_RETURN:
      return false;
    default:
      break;
    }
  }

  if (valid_symbols[MULTILINE_STRING] && scan_multiline_string(lexer))
    return true;

  return false;
}
