#include "trie.h"
#include <iostream>

using std::cout;
using std::endl;

int main() {
    Trie trie;
    trie.add_word("hello");

    cout << trie.contains_word("hello") << endl;
    cout << trie.contains_word("hell") << endl;
    cout << trie.contains_word("world") << endl;

    cout << trie.contains_prefix("hello") << endl;
    cout << trie.contains_prefix("hell") << endl;
    cout << trie.contains_prefix("world") << endl;
}
