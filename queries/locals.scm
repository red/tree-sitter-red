; Scopes
[
  (source_file)
  (function)
  (context)
] @local.scope

; Definitions

(function
  name: (_) @local.definition
  spec: (block (word) @local.definition)?)
(does
  name: (_) @local.definition)
(context
  name: (_) @local.definition)

; References
(word) @local.reference
(path) @local.reference
