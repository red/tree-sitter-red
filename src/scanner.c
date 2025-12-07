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

struct ScannerState {
  bool multiline_string;
};

#if __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(struct ScannerState) <=
                   TREE_SITTER_SERIALIZATION_BUFFER_SIZE,
               "Context too large");
#endif

enum TokenType {
  INFIX_OP,
  ERROR_SENTINEL,
};

#ifdef DEBUG
static const char *const symbol_names[] = {
    [INFIX_OP] = "$._infix_op",
};
#endif

void tree_sitter_external_scanner(reset)(void *payload) {
  struct ScannerState *context = payload;
  context->multiline_string = false;
}

void *tree_sitter_external_scanner(create)(void) {
  struct ScannerState *context = malloc(sizeof(struct ScannerState));
  tree_sitter_external_scanner(reset)(context);
  return context;
}

void tree_sitter_external_scanner(destroy)(void *payload) { free(payload); }

unsigned tree_sitter_external_scanner(serialize)(void *payload, char *buffer) {
  trace("serializing\n");
  *(struct ScannerState *)buffer = *(struct ScannerState *)payload;
  return sizeof(struct ScannerState);
}

void tree_sitter_external_scanner(deserialize)(void *payload,
                                               const char *buffer,
                                               unsigned length) {
  tree_sitter_external_scanner(reset)(payload);
  if (length != sizeof(struct ScannerState)) {
    return;
  }
  *(struct ScannerState *)payload = *(struct ScannerState *)buffer;
}

static void advance(TSLexer *lexer) { lexer->advance(lexer, false); }
static void skip(TSLexer *lexer) { lexer->advance(lexer, true); }

static void skip_spaces(TSLexer *lexer, const bool *valid_symbols) {
  while (iswspace(lexer->lookahead) && !lexer->eof(lexer)) {
    skip(lexer);
  }
}

#ifdef DEBUG
static bool trace_valid_symbols(const bool *valid_symbols) {
  for (int i = SCANNER_RESET; i < ERROR_SENTINEL; i++) {
    printf("valid_symbols[%s]: %s\n", symbol_names[i],
           valid_symbols[i] ? "true" : "false");
  }
  return false;
}
#define trace_valid_symbols(...) trace_valid_symbols(valid_symbols)
#else
#define trace_valid_symbols(...)
#endif

static bool scan_infix_op(TSLexer *lexer, const bool *valid_symbols) {
  if (!iswspace(lexer->lookahead))
    return false;

  skip_spaces(lexer, valid_symbols);

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
  struct ScannerState *context = (struct ScannerState *)payload;

  trace("==========\n");
  tracef("lookahead: %c @ %d\n", lexer->lookahead, lexer->get_column(lexer));
  trace_valid_symbols(valid_symbols);

  if (lexer->eof(lexer)) {
    return false;
  }

  if (valid_symbols[ERROR_SENTINEL]) {
    return false;
  }

  if (valid_symbols[INFIX_OP]) {
    return scan_infix_op(lexer, valid_symbols);
  }

  return false;
}
