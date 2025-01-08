#include "trie.h"

#include <string>

using std::make_unique;
using std::string;

void Trie::add_word(const string& word) {
    Trie* cur_trie = this;
    for (size_t i=0; i<word.length(); ++i) {
        if (!cur_trie->children.contains(word[i])) {
            cur_trie->children[word[i]] = make_unique<Trie>();
        }
        cur_trie = (cur_trie->children)[word[i]].get();
    }
    cur_trie->is_end = true;
}

bool Trie::contains_prefix(const string& word) {
    Trie* cur_trie = this;
    for (size_t i=0; i<word.length(); ++i) {
        if (!cur_trie->children.contains(word[i])) return false;
        cur_trie = (cur_trie->children)[word[i]].get();
    }
    return true;
}

bool Trie::contains_word(const string& word) {
    Trie* cur_trie = this;
    for (size_t i=0; i<word.length(); ++i) {
        if (!cur_trie->children.contains(word[i])) return false;
        cur_trie = (cur_trie->children)[word[i]].get();
    }
    return cur_trie->is_end;
}
