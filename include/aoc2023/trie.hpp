#include <stddef.h>
#include <string>
#include <queue>

class Trie
{
    static constexpr size_t ALPHABET_SIZE = 26;

public:
    struct Node
    {
        Node *children[ALPHABET_SIZE];
        int value;
        bool terminal;

        Node() : value(-1), terminal(false)
        {
            for (int i = 0; i < ALPHABET_SIZE; ++i)
            {
                children[i] = nullptr;
            }
        }
    };

    void insert(std::string key, int val)
    {
        Node *x = &root;
        for (const auto &c : key)
        {
            size_t idx = c - 97;
            if (x->children[idx] == nullptr)
            {
                x->children[idx] = new Node();
            }
            x = x->children[idx];
        }
        x->value = val;
        x->terminal = true;
    }

    int search(const std::string &s) const
    {
        const Node *x = &root;
        for (const auto &c : s)
        {
            if ((c >= 97) && (c <= 122) && x->children[c - 97])
            {
                x = x->children[c - 97];
                if (x && x->terminal)
                {
                    return x->value;
                }
            }
            else
            {
                break;
            }
        }
        return -1;
    }

    void print()
    {
        std::queue<Node *> cue;

        cue.push(&root);

        while (!cue.empty())
        {
            Node *x = cue.front();
            cue.pop();
            for (const auto &node : x->children)
            {
                if (node)
                {
                    std::cout << node->value << " ";
                    cue.push(node);
                }
            }
            std::cout << std::endl;
        }
    }

private:
    Node root{};
};