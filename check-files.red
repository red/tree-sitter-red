Red [
    Title:   "Tree-sitter parse runner for Red files"
    Version: 0.1
]

; Collect all .red and .reds files recursively from a starting directory
collect-red-files: func [start [file!] /local results][
    results: make block! 100
    foreach entry read start [
        either dir? entry [
            append results collect-red-files rejoin [start entry]
        ][
            if any [
                equal? suffix? entry ".red"
                equal? suffix? entry ".reds"
            ][
                append results rejoin [start entry]
            ]
        ]
    ]
    results
]

; Build and run the tree-sitter command for each file
run-tree-sitter: func [files [block!] /local output path cmd][
	output: make string! 1000
    foreach f files [
        ; Convert to local OS path and wrap in quotes to handle spaces
        path: to-local-file f
        cmd: rejoin ["tree-sitter parse " {"} path {"}]
        if 0 <> call/wait/shell/output cmd output [
        	print [path find/last output "(ERROR "]
        ]
        clear output
    ]
]

; Entry point: use script arg as root folder if provided, else current directory
root: either not empty? system/script/args [
    dirize to-red-file system/script/args
][
    print "Missing folder: red.exe ./check-files.red folder"
    halt
]
?? root
print rejoin ["Starting in: " to-local-file root]
files: collect-red-files root
print rejoin ["Found " mold length? files " file(s)."]
run-tree-sitter files
print "Done."
