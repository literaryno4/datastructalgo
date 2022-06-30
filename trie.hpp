//
// trie tree. this data structure is very useful in auto completion and spell checking
//
#include <vector>
#include <string>

class Trie {
    std::vector<Trie*> children_;
    bool isEnd_;
    Trie* endTrie (const std::string& word) {
        Trie* t = this;
        for (auto& c: word) {
            unsigned idx = c - 'a';
            if (t->children_[idx] == nullptr) {
                return nullptr;
            }
            t = t->children_[idx];
        }
        return t;
    }

public:
    Trie() : children_(26, nullptr), isEnd_(false) {}
    void insert(const std::string& word) {
        Trie* t = this;
        for (auto& c: word) {
            unsigned idx = c - 'a';
            if (t->children_[idx] == nullptr) {
                t->children_[idx] = new Trie;
            }
            t = t->children_[idx];
        }
        t->isEnd_ = true;
    }

    bool search(const std::string& word) {
        Trie* t = endTrie(word);
        return t != nullptr && t->isEnd_;
    }

    bool startWith(const std::string& prefix) {
        return endTrie(prefix);
    }
};
