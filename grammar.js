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
  externals: ($) => [
    $._infix_op,
    $.hexa,
    $.raw_string,
    $.multiline_string,
    $.error_sentinel,
  ],
  word: ($) => $._word,

  //conflicts: ($) => [[$.path, $.set_path]],

  rules: {
    /* General rules */
    source_file: ($) => repeat($._statement_seq),
    _statement_seq: ($) => choice($.include, $._expression),
    include: ($) => seq("#include", $.file),

    /* Comments */
    comment: (_) => /;.*/,

    /* expressions */
    _expression: ($) => choice($._complex_expression, $._simple_expression),
    _simple_expression: ($) => choice($._literal, $.infix),

    infix: ($) =>
      prec.left(
        1,
        seq(
          field("left", $._simple_expression),
          field("operator", $._infix_op),
          field("right", $._simple_expression),
        ),
      ),

    _literal: ($) =>
      choice(
        $.hexa,
        $.escaped_value,
        $.raw_string,
        $.multiline_string,
        $.word,
        $.lit_word,
        $.get_word,
        $.set_word,
        $.path,
        $.lit_path,
        $.get_path,
        $.set_path,
        $.boolean,
        $.number,
        $.pair,
        $.tuple,
        $.char,
        $.file,
        $.string,
        $.issue,
        $.binary,
        $.map,
        $.refinement,
        $.tag,
        $.ref,
        $.email,
        $.point,
      ),

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

    point: ($) =>
      seq("(", $.number, ",", $.number, optional(seq(",", $.number)), ")"),

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

    ref: (_) => /@[^\s\[\]\(\)\{\}@#$;,'"=<>^]*/,
    issue: (_) => /#[^\s\[\]\(\)\{\}@;"<>:]+/,
    refinement: (_) => /\/[^\s\[\]\(\)\{\}@;"<>:]+/,
    email: (_) => {
      const char = /[^\s\[\]\(\)\{\}@;:<"]/;
      return token(seq(repeat1(char), "@", repeat(char)));
    },

    file: ($) => seq("%", choice($.string, $.file_content)),
    file_content: (_) => token.immediate(prec(1, /[^\[\]\(\)\{\}@:;"\n]+/)),

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

    escaped_value: (_) => seq("#(", /[A-Za-z\-!]{3,20}/, ")"),

    _word: (_) =>
      /[^\p{White_Space}\d'\/\\,\[\]\(\)\{\}"#%\$@:;][^\p{White_Space}\/\\,\[\]\(\)\{\}"#%\$@:;]*/u,
    word: ($) => choice($._word, /\/+/),
    lit_word: ($) => seq("'", $.word),
    get_word: ($) => seq(":", $.word),
    set_word: ($) => prec(1, seq($.word, ":")),

    _path_element: ($) =>
      choice(
        $.boolean,
        $.number,
        $.pair,
        $.tuple,
        $.char,
        $.file,
        $.string,
        $.issue,
        $.binary,
        $.map,
        $.word,
        $.lit_word,
        $.get_word,
        $.tag,
        $.ref,
      ),
    path: ($) => seq($.word, repeat1(seq("/", $._path_element))),
    lit_path: ($) => seq("'", $.word, repeat1(seq("/", $._path_element))),
    get_path: ($) => seq(":", $.word, repeat1(seq("/", $._path_element))),
    set_path: ($) =>
      prec(2, seq($.word, repeat1(seq("/", $._path_element)), ":")),

    _binary_base_2: ($) =>
      seq("2#{", repeat(choice(/(?:[01]\s*){8}/, $.comment)), "}"),
    _binary_base_16: ($) =>
      seq(
        optional("16"),
        "#{",
        repeat(choice(/[0-9a-fA-F]{2}/, $.comment)),
        "}",
      ),
    _binary_base_64: ($) =>
      seq(
        "64#{",
        repeat(choice(/[A-Za-z0-9\+\/]/, $.comment)),
        optional("="),
        optional("="),
        "}",
      ),
    binary: ($) =>
      choice($._binary_base_2, $._binary_base_16, $._binary_base_64),

    map: ($) => seq("#[", repeat(seq($._literal, $._literal)), "]"),
    tag: (_) =>
      token(
        prec(
          2,
          seq(
            /<[^\s\[\]\(\)\{\};"<>=]/,
            repeat(choice(/".*"/, /'.*'/, /[^>]/)),
            ">",
          ),
        ),
      ),

    _complex_expression: ($) => choice($.while, $.loop),

    block: ($) => seq("[", repeat($._expression), "]"),
    paren: ($) => seq("(", repeat($._simple_expression), ")"),

    while: ($) => seq("while", $.block, $.block),

    loop: ($) => seq("loop", $._simple_expression, $.block),
  },
});
