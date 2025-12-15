;; Operators

(infix_op) @operator

;; Types

((word) @type
    (#match? @type "^.*!$"))

;; Keywords
((word) @keyword
    (#match? @keyword
        "(?i)^(about|ajoin|alert|also|alter|any\\-object\\?|any\\-path\\?|append|apply|array|as\\-pair|ascii\\?|ask|assert|attempt|bb|body\\-of|brightness\\?|build\\-attach\\-body|build\\-markup|build\\-tag|cause\\-error|cd|center\\-face|change\\-dir|charset|choose|clean\\-path|clear\\-face|clear\\-fields|closure|closure\\?|collect|collect\\-words|component\\?|confine|confirm|cvs\\-date|cvs\\-version|dbug|decode\\-cgi|decode\\-url|default|deflag\\-face|delete|delete\\-dir|deline|delta\\-time|desktop|dir\\?|dirize|dispatch|do\\-boot|do\\-events|do\\-face|do\\-face\\-alt|do\\-thru|dt|dump\\-face|dump\\-obj|dump\\-pane|echo|edge\\-size\\?|editor|emailer|enline|exists\\-thru\\?|exists\\?|extract|find\\-key\\-face|find\\-window|first+|flag\\-face|flag\\-face\\?|flash|focus|foo|foo10|foo9|for|forall|forever|forskip|found\\?|funct|get\\-face|get\\-net\\-info|get\\-path\\?|get\\-style|has|help|hide\\-popup|hilight\\-all|hilight\\-text|import\\-email|in\\-dir|in\\-window\\?|info\\?|inform|input|insert\\-event\\-func|inside\\?|install|invalid\\-utf\\?|join|last\\?|latin1\\?|launch\\-thru|layout|license|link\\-app\\?|link\\-relative\\-path|link\\?|list\\-dir|load\\-image|load\\-stock|load\\-stock\\-block|load\\-thru|ls|make\\-dir|make\\-face|map\\-each|mod|modified\\?|modulo|more|move|net\\-error|notify|offset\\?|open\\-events|outside\\?|overlap\\?|parse\\-email\\-addrs|parse\\-header|parse\\-header\\-date|parse\\-xml|path\\-thru|probe|protect\\-system|pwd|quote|read\\-cgi|read\\-net|read\\-thru|reflect|reform|rejoin|remold|remove\\-event\\-func|rename|repend|replace|request|request\\-color|request\\-date|request\\-dir|request\\-download|request\\-file|request\\-list|request\\-pass|request\\-text|resend|reset\\-face|resize\\-face|resolve|rm|round|save\\-user|scalar\\?|screen\\-offset\\?|scroll\\-drag|scroll\\-face|scroll\\-para|send|set\\-face|set\\-font|set\\-net|set\\-para|set\\-style|set\\-user\\-name|show\\-popup|sign\\?|size\\?|source|span\\?|spec\\-of|speed\\?|split\\-path|stylize|suffix\\?|swap|take|throw\\-error|throw\\-on\\-error|title\\-of|to\\-binary|to\\-bitset|to\\-block|to\\-char|to\\-closure|to\\-datatype|to\\-date|to\\-decimal|to\\-email|to\\-error|to\\-file|to\\-function|to\\-get\\-path|to\\-get\\-word|to\\-hash|to\\-idate|to\\-image|to\\-integer|to\\-issue|to\\-itime|to\\-library|to\\-list|to\\-lit\\-path|to\\-lit\\-word|to\\-logic|to\\-map|to\\-money|to\\-none|to\\-pair|to\\-paren|to\\-path|to\\-port|to\\-refinement|to\\-relative\\-file|to\\-set\\-path|to\\-set\\-word|to\\-string|to\\-tag|to\\-time|to\\-tuple|to\\-typeset|to\\-url|to\\-word|true\\?|types\\-of|typeset\\?|undirize|unfocus|uninstall|unlight\\-text|unview|upgrade|Usage|utf\\?|values\\-of|vbug|view|viewed\\?|viewtop|what|what\\-dir|while|loop|win\\-offset\\?|within\\?|words\\-of|write\\-user|access\\-os|alias|all|any|arccosine|arcsine|arctangent|as|AS|as\\-binary|as\\-string|bind|bind\\?|bound\\?|break|browse|call|caret\\-to\\-offset|case|catch|checksum|close|comment|compose|compress|continue|connected\\?|construct|cosine|create\\-link|crypt\\-strength\\?|debase|declare|decloak|decompress|dehex|detab|dh\\-compute\\-key|dh\\-generate\\-key|dh\\-make\\-key|difference|disarm|do|do\\-browser|draw|dsa\\-generate\\-key|dsa\\-make\\-key|dsa\\-make\\-signature|dsa\\-verify\\-signature|either|else|enbase|encloak|entab|exclude|exit|exp|foreach|form|free|get|get\\-env|get\\-modes|halt|hide|hsv\\-to\\-rgb|if|in|input\\?|intersect|launch|list\\-env|load|local\\-request\\-file|log\\-10|log\\-2|log\\-e|lowercase|maximum\\-of|minimum\\-of|mold|native|new\\-line|new\\-line\\?|not|now|offset\\-to\\-caret|open|parse|prin|print|protect|q|query|quit|quit\\-return|read|read\\-io|recycle|reduce|remove\\-each|repeat|reverse|rgb\\-to\\-hsv|rsa\\-encrypt|rsa\\-generate\\-key|rsa\\-make\\-key|run|save|script\\?|secure|set|set\\-env|set\\-modes|shift|show|sine|size\\-text|square\\-root|stats|switch|tangent|textinfo|throw|to\\-hex|to\\-local\\-file|to\\-rebol\\-file|trace|try|type\\?|unbind|union|unique|unless|unprotect|unset|until|update|uppercase|use|value\\?|wait|with|write|write\\-io|abs|absolute|action\\?|add|and~|any\\-block\\?|any\\-function\\?|any\\-series\\?|any\\-string\\?|any\\-type\\?|any\\-word\\?|at|back|binary\\?|bitset\\?|block\\?|change|char\\?|clear|complement|copy|copy*|cp|datatype\\?|date\\?|decimal\\?|divide|eighth|email\\?|empty\\?|equal\\?|error\\?|even\\?|event\\?|fifth|file\\?|find|first|fourth|function\\?|get\\-word\\?|greater\\-or\\-equal\\?|greater\\?|hash\\?|head|head\\?|image\\?|index\\?|insert|integer\\?|issue\\?|last|length\\?|lesser\\-or\\-equal\\?|lesser\\?|library\\?|list\\?|lit\\-path\\?|lit\\-word\\?|logic\\?|make|map\\?|max|maximum|min|minimum|money\\?|multiply|native\\?|negate|negative\\?|next|ninth|none\\?|not\\-equal\\?|number\\?|object\\?|odd\\?|op\\?|or~|pair\\?|paren\\?|path|path\\?|pick|poke|port\\?|positive\\?|power|random|refinement\\?|remainder|remove|routine\\?|same\\?|second|select|series\\?|set\\-path\\?|set\\-word\\?|seventh|sixth|skip|sort|strict\\-equal\\?|strict\\-not\\-equal\\?|string\\?|struct\\?|subtract|tag\\?|tail|tail\\?|tenth|third|time\\?|to|trim|tuple\\?|unset\\?|url\\?|word\\?|xor~|zero\\?|backslash|backspace|bs|cr|escape|lf|newline|newpage|null|slash|tab|sp|space|crlf|dot|yes|no|on|off|return)$"
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
] @number

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

(path (word) @none)
(set_path (path (word) @variable) @variable) @variable
(get_path (path (word) @variable) @variable) @variable
(lit_path (path (word) @constant) @constant) @constant

;; Comments

(comment) @comment

;; Functions

(function
    name: (set_word) @function.definition
    func: _ @keyword)

(function
    name: (set_word) @function.definition
    func: _ @keyword
    spec: (block (refinement) @emphasis
        (#eq? @emphasis "/local"))?)

(function
    name: (set_path (path (word) @function.definition) @function.definition) @function.definition
    func: _ @keyword
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
    ctx: _ @keyword)

;; Errors

(ERROR) @error
