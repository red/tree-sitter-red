;; Operators

(infix
  operator: _ @operator)

;; Include

(include) @module

;; Types

(type_block (type_word) @type)

(type_block (type_word) @type.builtin
 (#any-of? @type.builtin
     "datatype!" "unset!" "none!" "logic!" "block!" "paren!" "string!" "file!" "url!" "char!" "integer!" "float!" "word!" "set-word!" "lit-word!" "get-word!" "refinement!" "issue!" "native!" "action!" "op!" "function!" "path!" "lit-path!" "set-path!" "get-path!" "routine!" "bitset!" "object!" "typeset!" "error!" "vector!" "hash!" "pair!" "percent!" "tuple!" "map!" "binary!" "time!" "tag!" "email!" "handle!" "date!" "port!" "money!" "ref!" "point2D!" "point3D!" "IPv6!" "image!" "event!" "closure!" "slice!"
))

;; Keywords
(keywords) @keyword
[ "func" "function" ] @keyword.function
[ "while" "loop" ] @keyword.repeat

;; Delimiters

[
  ","
] @punctuation.delimiter

[
  "(" ")"
  "[" "]"
] @punctuation.bracket

;; Literals

(string) @string
(file) @string
(multiline_string) @string
(escaped_char) @string.escape

(number) @number
(boolean) @boolean
(char) @character

;; Comments

(comment) @comment @spell

;; Errors

(ERROR) @error
