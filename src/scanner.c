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
  ERROR_SENTINEL,
};

#ifdef DEBUG
static const char *const symbol_names[] = {
    [INFIX_OP] = "$._infix_op",
    [RED_HEXA] = "$.hexa",
    [RAW_STRING] = "$.raw_string",
    [MULTILINE_STRING] = "$.multiline_string",
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

/// Scan the raw string: /(%+)\{.*?\}\1/
static bool scan_raw_string(struct Scanner *scanner, TSLexer *lexer) {
  skip_spaces(lexer);

  // Step 1: count leading %
  int left = 0;
  while (lexer->lookahead == '%') {
    advance(lexer);
    left++;
  }
  if (left == 0)
    return false;

  // Step 2: require opening brace
  if (lexer->lookahead != '{')
    return false;
  advance(lexer);

  for (int delimiter_index = -1;;) {
    // If we hit EOF, consider the content to terminate there.
    // This forms an incomplete raw_string, and models the code well.
    if (lexer->eof(lexer)) {
      lexer->mark_end(lexer);
      lexer->result_symbol = RAW_STRING;
      return true;
    }

    if (delimiter_index >= 0) {
      if (delimiter_index == left) {
        lexer->mark_end(lexer);
        lexer->result_symbol = RAW_STRING;
        return true;
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
  return false;
}

static bool scan_multiline_string(struct Scanner *scanner, TSLexer *lexer) {
  skip_spaces(lexer);

  if (lexer->lookahead != '{')
    return false;
  advance(lexer);

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
        advance(lexer);
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
        advance(lexer);
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

static bool scan_infix_op(TSLexer *lexer) {
  if (!iswspace(lexer->lookahead))
    return false;

  skip_spaces(lexer);

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
  case '+':
  case '-':
  case '*':
  case '/':
    advance(lexer);
    break;
  default:
    find = false;
  }
  if (find) {
    if (!iswspace(lexer->lookahead))
      return false;
    lexer->mark_end(lexer);
    lexer->result_symbol = INFIX_OP;
    return true;
  }
  return false;
}

bool tree_sitter_external_scanner(scan)(void *payload, TSLexer *lexer,
                                        const bool *valid_symbols) {
  struct Scanner *scanner = (struct Scanner *)payload;

  trace("==========\n");
  tracef("lookahead: %d\n", lexer->lookahead);
  trace_valid_symbols(valid_symbols);

  if (lexer->eof(lexer)) {
    return false;
  }

  if (valid_symbols[ERROR_SENTINEL]) {
    return false;
  }

  if (valid_symbols[RED_HEXA]) {
    skip_spaces(lexer);
    int count = 0;
    // 2 - 8 characters
    while (iswxdigit(lexer->lookahead) && count < 8) {
      advance(lexer);
      count++;
    }
    if (count >= 2 && lexer->lookahead == 'h') {
      advance(lexer);
      lexer->mark_end(lexer);
      lexer->result_symbol = RED_HEXA;
      trace("find hexa !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return true;
    }
  }

  if (valid_symbols[INFIX_OP] && scan_infix_op(lexer))
    return true;

  if (valid_symbols[RAW_STRING] && scan_raw_string(scanner, lexer))
    return true;
  if (valid_symbols[RAW_STRING] && scan_multiline_string(scanner, lexer))
    return true;

  return false;
}
