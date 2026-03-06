;; Operators

(infix_op) @operator

(word) @variable

;; Types

((word) @type
    (#match? @type "^.*!$"))

;; Keywords
((word) @keyword
    (#match? @keyword
        "(?i)^(\\?|\\?\\?|a\\-an|about|absolute|acos|action\\?|add|alert|all|all\\-word\\?|also|alter|and~|any|any\\-block\\?|any\\-function\\?|any\\-interesting\\?|any\\-list\\?|any\\-object\\?|any\\-path\\?|any\\-point\\?|any\\-string\\?|any\\-word\\?|append|apply|arccosine|arcsine|arctangent|arctangent2|as|as\\-color|as\\-ipv4|as\\-money|as\\-pair|as\\-point2D|as\\-point3D|as\\-rgba|asin|ask|at|atan|atan2|attempt|average|back|binary\\?|bind|bitset\\?|block\\?|body\\-of|break|browse|call|caret\\-to\\-offset|case|catch|cause\\-error|cd|center\\-face|change|change\\-dir|char\\?|charset|checksum|class\\-of|clean\\-path|clear|clear\\-reactions|clock|close|collect|collect\\-calls|comment|complement|complement\\?|compose|compress|construct|context|context\\?|continue|copy|cos|cosine|count\\-chars|create|create\\-dir|datatype\\?|date\\?|debase|debug|debug\\-info\\?|decode\\-url|decompress|deep\\-reactor|dehex|delete|difference|dir|dir\\?|dirize|distance\\?|divide|do|do\\-actor|do\\-events|do\\-file|do\\-no\\-sync|do\\-safe|do\\-thru|does|draw|dt|dump\\-face|dump\\-reactions|either|ellipsize\\-at|email\\?|empty\\?|enbase|encode\\-url|enhex|equal\\?|error\\?|eval\\-set\\-path|even\\?|event\\?|exclude|exists\\-thru\\?|exists\\?|exit|exp|expand|expand\\-directives|extend|extract|extract\\-boot\\-args|face\\?|fetch\\-help|fifth|file\\?|find|find\\-flag\\?|first|flip\\-exe\\-flag|float\\?|forall|foreach|foreach\\-face|forever|form|fourth|frame\\-index\\?|func|function|function\\?|get|get\\-current\\-dir|get\\-current\\-screen|get\\-env|get\\-face\\-pane|get\\-focusable|get\\-path\\?|get\\-scroller|get\\-sys\\-words|get\\-word\\?|greater\\-or\\-equal\\?|greater\\?|halt|handle\\?|has|hash\\?|head|head\\?|help|help\\-string|hex\\-to\\-rgb|if|image\\?|immediate\\?|in|index\\?|input|input\\-stdin|insert|insert\\-event\\-func|integer\\?|intersect|is|issue\\?|keys\\-of|last|last\\-lf\\?|last\\?|layout|length\\?|lesser\\-or\\-equal\\?|lesser\\?|link\\-sub\\-to\\-parent|link\\-tabs\\-to\\-parent|list\\-dir|list\\-env|lit\\-path\\?|lit\\-word\\?|ll|load|load\\-csv|load\\-json|load\\-thru|log\\-10|log\\-2|log\\-e|logic\\?|loop|lowercase|ls|make|make\\-dir|make\\-face|map\\?|math|max|metrics\\?|min|mod|modify|modulo|mold|money\\?|move|multiply|NaN\\?|native\\?|negate|negative\\?|new\\-line|new\\-line\\?|next|no\\-react|none\\?|normalize\\-dir|not|not\\-equal\\?|now|number\\?|object|object\\?|odd\\?|offset\\-to\\-caret|offset\\-to\\-char|offset\\?|op\\?|open|open\\?|or~|os\\-info|overlap\\?|pad|pair\\?|paren\\?|parse|parse\\-trace|path\\-thru|path\\?|percent\\?|pick|pick\\-stack|planar\\?|point2D\\?|point3D\\?|poke|positive\\?|power|prin|print|probe|profile|put|pwd|q|query|quit|quit\\-return|quote|random|react|react\\?|reactor|read|read\\-clipboard|read\\-stdin|read\\-thru|recycle|red\\-complete\\-input|reduce|ref\\?|refinement\\?|reflect|register\\-scheme|rejoin|relate|remainder|remove|remove\\-each|remove\\-event\\-func|rename|repeat|repend|replace|request\\-dir|request\\-file|request\\-font|return|reverse|round|routine|routine\\?|rtd\\-layout|same\\?|save|scalar\\?|scan|second|select|series\\?|set|set\\-current\\-dir|set\\-env|set\\-flag|set\\-focus|set\\-path\\?|set\\-quiet|set\\-slot\\-quiet|set\\-word\\?|shift|shift\\-left|shift\\-logical|shift\\-right|show|show\\-memory\\-stats|sign\\?|sin|sine|single\\?|size\\-text|size\\?|skip|sort|source|spec\\-of|split|split\\-path|sqrt|square\\-root|stack\\-size\\?|stats|stop\\-events|stop\\-reactor|strict\\-equal\\?|string\\?|subtract|suffix\\?|sum|swap|switch|tag\\?|tail|tail\\?|take|tan|tangent|third|throw|time\\-it|time\\?|to|to\\-binary|to\\-bitset|to\\-block|to\\-char|to\\-csv|to\\-date|to\\-email|to\\-file|to\\-float|to\\-get\\-path|to\\-get\\-word|to\\-hash|to\\-hex|to\\-image|to\\-integer|to\\-issue|to\\-json|to\\-lit\\-path|to\\-lit\\-word|to\\-local\\-date|to\\-local\\-file|to\\-logic|to\\-map|to\\-money|to\\-none|to\\-pair|to\\-paren|to\\-path|to\\-percent|to\\-point2D|to\\-point3D|to\\-red\\-file|to\\-ref|to\\-refinement|to\\-set\\-path|to\\-set\\-word|to\\-string|to\\-tag|to\\-time|to\\-tuple|to\\-typeset|to\\-unset|to\\-url|to\\-UTC\\-date|to\\-word|trace|tracing\\?|transcode|transcode\\-trace|trim|try|tuple\\?|type\\?|typeset\\?|union|unique|unless|unset|unset\\?|until|unview|update|update\\-font\\-faces|uppercase|url\\?|value\\?|values\\-of|vector\\?|view|wait|what|what\\-dir|while|within\\?|word\\?|words\\-of|write|write\\-clipboard|write\\-stdout|xor~|zero\\?)$"
))

;; Delimiters

[
  ","
] @punctuation.delimiter

[
  "(" ")"
  "[" "]"
  "{" "}"
] @punctuation.bracket

;; Literals

[
  (string)
  (file)
] @string

(multiline_string) @text.literal

[
  (escaped_char)
  (construction)
] @string.escape

[
  (number)
  (hexa)
  (pair)
  (point)
  (money)
  (date)
  (time)
  (tuple)
  (ipv6)
  (ipv6_address)
] @number

(binary) @constant

(boolean) @boolean
(char) @string.special.symbol

(issue) @preproc
(tag) @tag
(set_word) @variable
(url) @link_uri
(email) @link_text
(ref) @label

(get_word (word) @variable) @variable
(lit_word (word) @constant) @constant

(path (_)) @variable
(path (word) @variable) @variable
(set_path (path (word) @variable) @variable) @variable
(get_path (path (word) @variable) @variable) @variable
(lit_path (path (word) @constant) @constant) @constant

;; Comments

(comment) @comment

;; Functions

(function
    name: (set_word) @function.definition
    key: _ @keyword)

(function
    name: (set_word) @function.definition
    key: _ @keyword
    spec: (block (refinement) @emphasis
        (#eq? @emphasis "/local"))?)

(function
    name: (set_path (path (word) @function.definition) @function.definition) @function.definition
    key: _ @keyword
    spec: (block (refinement) @emphasis
        (#eq? @emphasis "/local"))?)

(does
    name: (set_word) @function.definition
    key: _ @keyword)

(does
    name: (set_path (path (word) @function.definition) @function.definition) @function.definition
    key: _ @keyword)

;; Contexts & Objects
(context
    name: (_) @variable
    key: _ @keyword)

;; Errors

(invalid_token _ @error) @error
(ERROR) @error
