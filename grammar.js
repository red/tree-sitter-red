/**
 * @file A tree-sitter grammar for the Red programming language
 * @author qtxie <xqtxyz@gmail.com>
 * @license Apache-2.0
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
  name: "red",

  extras: ($) => [$.comment, /\s/],
  externals: ($) => [$._infix_op, $.error_sentinel],
  word: ($) => $.word,

  //conflicts: ($) => [[$.path, $.set_path]],

  rules: {
    /* General rules */
    source_file: ($) => repeat($._statement_seq),
    _statement_seq: ($) => choice($.include, $._expression),
    /* Comments */
    comment: (_) => /;.*/,
    /* Include */
    include: ($) => seq("#include", $.file),
    file: ($) => seq("%", choice($.string, $.file_content)),
    file_content: ($) => token.immediate(prec(1, /[^\[\]\(\){}@:;"\n]+/)),
    string: ($) =>
      seq(
        '"',
        repeat(
          choice(
            alias(token.immediate(prec(1, /[^\^\\"\n]+/)), $.string_content),
            $.escaped_char,
          ),
        ),
        '"',
      ),
    escaped_char: (_) =>
      token(
        prec(
          1,
          seq(
            "^",
            choice(
              "/",
              "-",
              "~",
              "^",
              "{",
              "}",
              '"',
              /[a-fA-F]/,
              "(null)",
              "(back)",
              "(tab)",
              "(line)",
              "(page)",
              "(esc)",
              "(del)",
              /\([0-9a-fA-F]{1,6}\)/,
            ),
          ),
        ),
      ),

    _expression: ($) => choice($._complex_expression, $._simple_expression),
    _simple_expression: ($) =>
      choice(
        $._literal,
        $.word,
        $.lit_word,
        $.get_word,
        $.set_word,
        $.path,
        $.lit_path,
        $.get_path,
        $.set_path,
        $.binary_expression,
      ),
    _path_element: ($) => choice($._literal, $.word, $.lit_word, $.get_word),

    binary_expression: ($) =>
      prec.left(
        1,
        seq(
          field("left", $._simple_expression),
          field("operator", $._infix_op),
          field("right", $._simple_expression),
        ),
      ),

    _literal: ($) =>
      choice($.boolean, $.number, $.pair, $.tuple, $.char, $.string),

    boolean: (_) => choice("true", "false"),

    number: (_) => {
      const separator = "'";
      const decimal = /[0-9]/;
      const decimalDigits = seq(
        repeat1(decimal),
        repeat(seq(separator, repeat1(decimal))),
      );
      return token(
        choice(
          seq(
            optional(/[-\+]/),
            decimalDigits,
            optional(seq(".", decimalDigits)),
            optional(seq(/[eE]/, optional(seq(optional(/[-\+]/), decimal)))),
          ),
          seq(optional(/[-\+]/), choice(/1.#inf/i, /1.#nan/i)),
        ),
      );
    },

    pair: (_) => {
      const separator = "'";
      const decimal = /[0-9]/;
      const integer = seq(
        optional(/[-\+]/),
        repeat1(decimal),
        repeat(seq(separator, repeat1(decimal))),
      );
      return token(seq(integer, /[xX]/, integer));
    },

    tuple: (_) => {
      const byte = choice(
        seq("25", /[0-5]/),
        seq("2", /[0-4]/, /\d/),
        seq("1", /\d{2}/),
        seq(optional("0"), /[1-9]/, /\d/),
        seq(optional("0"), optional("0"), /\d/),
        "0",
      );
      const dot_byte = seq(".", byte);
      return token(
        seq(
          byte,
          dot_byte,
          dot_byte,
          optional(dot_byte),
          optional(dot_byte),
          optional(dot_byte),
          optional(dot_byte),
          optional(dot_byte),
          optional(dot_byte),
          optional(dot_byte),
          optional(dot_byte),
          optional(dot_byte),
        ),
      );
    },

    char: ($) =>
      seq('#"', choice($.escaped_char, token.immediate(/[^"\^]/)), '"'),

    word: ($) =>
      /[^\p{White_Space}\d'\/\\,\[\]\(\)\{\}"#%\$@:;][^\p{White_Space}\/\\,\[\]\(\)\{\}"#%\$@:;]*/u,
    lit_word: ($) => seq("'", $.word),
    get_word: ($) => seq(":", $.word),
    set_word: ($) => prec(1, seq($.word, ":")),

    path: ($) => seq($.word, repeat1(seq("/", $._path_element))),
    lit_path: ($) => seq("'", $.word, repeat1(seq("/", $._path_element))),
    get_path: ($) => seq(":", $.word, repeat1(seq("/", $._path_element))),
    set_path: ($) =>
      prec(2, seq($.word, repeat1(seq("/", $._path_element)), ":")),

    _complex_expression: ($) => choice($.while, $.loop),

    block: ($) => seq("[", repeat($._expression), "]"),

    while: ($) => seq("while", $.block, $.block),

    loop: ($) => seq("loop", $._simple_expression, $.block),
  },
});
