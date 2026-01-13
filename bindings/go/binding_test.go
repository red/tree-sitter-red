package tree_sitter_red_test

import (
	"testing"

	tree_sitter "github.com/tree-sitter/go-tree-sitter"
	tree_sitter_red "github.com/red/tree-sitter-red/bindings/go"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_red.Language())
	if language == nil {
		t.Errorf("Error loading Red grammar")
	}
}
