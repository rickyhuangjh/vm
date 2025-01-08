#ifndef TRIE_H
#define TRIE_H

#include <memory>
#include <string>
#include <unordered_map>


using std::unique_ptr;
using std::unordered_map;
using std::string;

class Trie {
private:
    unordered_map<char, unique_ptr<Trie>> children;
    bool is_end;
public:
    void add_word(const string& word);
    bool contains_prefix(const string& word);
    bool contains_word(const string& word);

};


#endif
