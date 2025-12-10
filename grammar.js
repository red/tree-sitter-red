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
        $.money,
        $.time,
        $.date,
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
            optional("%"),
          ),
          seq(optional(/[-\+]/), choice(/1.#inf/i, /1.#nan/i), optional("%")),
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

    money: (_) =>
      token(
        seq(
          optional(/[-\+]/),
          optional(/[A-Za-z]{3}/),
          "$",
          repeat1(/\d/),
          repeat(seq("'", repeat1(/\d/))),
          optional(seq(".", repeat1(/\d/))),
        ),
      ),

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

    time: (_) => {
      const decimal_number = token(seq(".", /\d{1,9}/));
      return prec(
        1,
        token(
          seq(
            /\d+:\d+/,
            optional(
              choice(decimal_number, seq(/:\d+/, optional(decimal_number))),
            ),
          ),
        ),
      );
    },

    date: (_) => {
      // Basic components
      const year = /\d{3,4}/;
      const month_num = /\d{1,2}/;
      const day = /\d{1,2}/;
      const decimal_number = token(seq(".", /\d{1,9}/));

      const month_name = token(
        choice(
          /Jan(uary)?/i,
          /Feb(ruary)?/i,
          /Mar(ch)?/i,
          /Apr(il)?/i,
          /May/i,
          /Jun(e)?/i,
          /Jul(y)?/i,
          /Aug(ust)?/i,
          /Sep(t(ember)?)?/i,
          /Oct(ober)?/i,
          /Nov(ember)?/i,
          /Dec(ember)?/i,
        ),
      );
      const month = choice(month_num, month_name);

      const timezone = seq(
        token(
          choice(
            "Z",
            seq(
              choice("+", "-"),
              choice(/\d{4}/, seq(/\d{1,2}/, optional(/:\d{2}/))),
            ),
          ),
        ),
      );

      const _time = prec(
        1,
        token(
          seq(
            /[T\/]\d+:\d+/,
            optional(
              choice(decimal_number, seq(/:\d+/, optional(decimal_number))),
            ),
          ),
        ),
      );
      const time_z = prec(2, seq(_time, optional(timezone)));

      // date
      const ymd_dash = token(seq(year, "-", month, "-", day));
      const ymd_slash = token(seq(year, "/", month, "/", day));
      const dmy_dash = token(seq(day, "-", month, "-", choice(year, day)));
      const dmy_slash = token(seq(day, "/", month, "/", choice(year, day)));
      const yyyymmdd = token(
        seq(
          /\d{8}T/,
          choice(
            seq(/\d{6}/, optional(decimal_number), optional(timezone)),
            /\d{4}Z/,
          ),
        ),
      );
      const yyyywww = token(
        seq(
          year,
          "-",
          choice(seq("W", /\d{2}/, optional(seq("-", /[1-9]/))), /\d{3}/),
        ),
      );

      return choice(
        prec(
          2,
          token(
            seq(
              choice(ymd_dash, ymd_slash, dmy_dash, dmy_slash, yyyywww),
              optional(time_z),
            ),
          ),
        ),
        yyyymmdd,
      );
    },

    char: ($) =>
      seq('#"', choice($.escaped_char, token.immediate(/[^"\^]/)), '"'),

    ref: (_) => /@[^\s\[\]\(\)\{\}@#$;,'"=<>^]*/,
    issue: (_) => /#[^\s\[\]\(\)\{\}@;"<>:]+/,
    refinement: (_) => seq("/", /[^\s\/\\,\[\]\(\)\{\}"#%\$@:;<>]+/),
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
        $.path,
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
    _path_part: ($) => seq("/", $._path_element),
    path: ($) => prec(1, seq($.word, $._path_part)),
    lit_path: ($) => prec(1, seq("'", $.word, $._path_part)),
    get_path: ($) => prec(1, seq(":", $.word, $._path_part)),
    set_path: ($) => prec(2, seq($.word, $._path_part, ":")),

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

    _complex_expression: ($) =>
      choice($.paren, $.block, $.while, $.loop, $.function),

    block: ($) => seq("[", repeat($._expression), "]"),
    paren: ($) => seq("(", repeat($._simple_expression), ")"),

    while: ($) => seq(choice("while", "While", "WHILE"), $.block, $.block),

    loop: ($) =>
      seq(choice("loop", "LOOP", "Loop"), $._simple_expression, $.block),

    function: ($) =>
      seq(
        choice("func", "Func", "FUNC", "function", "Function", "FUNCTION"),
        $.block,
        $.block,
      ),
  },
});
