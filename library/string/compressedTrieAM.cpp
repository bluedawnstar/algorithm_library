#include <memory.h>
#include <cassert>
#include <cstring>
#include <string>
#include <stack>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

using namespace std;

#include "compressedTrieAM.h"

/////////// For Testing ///////////////////////////////////////////////////////

#include <time.h>
#include <cassert>
#include <iostream>
#include <vector>
#include "../common/iostreamhelper.h"
#include "../common/profile.h"
#include "../common/rand.h"

#include "compressedTrie.h"

static void dump(CompressedTrieAM<>::Node* p, int level) {
    if (!p)
        return;

    for (int i = 0; i < level; i++)
        cout << "+-";
    cout << "text=";
    for (int i = 0; i < p->textLen; i++)
        cout << p->text[i];
    cout << ", leaf=" << p->leafCount << endl;
    for (int i = 0; i < 26; i++) {
        if (p->hasChild(i))
            dump(p->getChild(i), level + 1);
    }
}

static string makeRandomString(int n) {
    string s;
    s.reserve(n);
    for (int i = 0; i < n; i++)
        s.push_back(RandInt32::get() % 26 + 'a');
    return s;
}

void testCompressedTrieAM() {
    return; //TODO: if you want to test, make this line a comment.

    cout << "-- Memory Mapped Compressed Trie -------------------------------------" << endl;
    {
        CompressedTrieAM<> compTrie;

        const char* keys[] = { "the", "a", "there", "answer", "any", "by", "bye", "their" };
        for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
            compTrie.insert(keys[i], int(strlen(keys[i])));

        auto* x = compTrie.find("the", int(strlen("the")));
        assert(compTrie.find("the", int(strlen("the")))->leafCount == 1);
        assert(compTrie.find("these", int(strlen("these"))) == nullptr);
        x = compTrie.find("their", int(strlen("their")));
        assert(compTrie.find("their", int(strlen("their")))->leafCount == 1);
        assert(compTrie.find("thaw", int(strlen("thaw"))) == nullptr);

        compTrie.insert("the", int(strlen("the")));
        compTrie.insert("these", int(strlen("these")));
        compTrie.insert("their", int(strlen("their")));
        compTrie.insert("thaw", int(strlen("thaw")));

        assert(compTrie.find("th", int(strlen("th"))) == nullptr);
        assert(compTrie.find("the", int(strlen("the")))->leafCount == 2);
        assert(compTrie.find("these", int(strlen("these")))->leafCount == 1);
        assert(compTrie.find("their", int(strlen("their")))->leafCount == 2);
        assert(compTrie.find("thaw", int(strlen("thaw")))->leafCount == 1);

        //cout << "*** after insertion ***" << endl;
        //dump(&compTrie.mRoot, 0);

        compTrie.remove("the", int(strlen("the")));
        compTrie.remove("these", int(strlen("these")));
        compTrie.remove("their", int(strlen("their")));
        compTrie.remove("thaw", int(strlen("thaw")));

        assert(compTrie.find("the", int(strlen("the")))->leafCount == 1);
        assert(compTrie.find("these", int(strlen("these"))) == nullptr);
        assert(compTrie.find("their", int(strlen("their")))->leafCount == 1);
        assert(compTrie.find("thaw", int(strlen("thaw"))) == nullptr);

        //cout << "*** after removal ***" << endl;
        //dump(&compTrie.mRoot, 0);

        compTrie.insert("the", int(strlen("the")));
        compTrie.insert("these", int(strlen("these")));
        compTrie.insert("their", int(strlen("their")));
        compTrie.insert("thaw", int(strlen("thaw")));

        assert(compTrie.find("the", int(strlen("the")))->leafCount == 2);
        assert(compTrie.find("these", int(strlen("these")))->leafCount == 1);
        assert(compTrie.find("their", int(strlen("their")))->leafCount == 2);
        assert(compTrie.find("thaw", int(strlen("thaw")))->leafCount == 1);

        //cout << "*** after insertion ***" << endl;
        //dump(&compTrie.mRoot, 0);

        compTrie.erase("the", int(strlen("the")));
        compTrie.erase("these", int(strlen("these")));
        compTrie.erase("their", int(strlen("their")));
        compTrie.erase("thaw", int(strlen("thaw")));

        compTrie.erase("any", int(strlen("any")));

        assert(compTrie.find("the", int(strlen("the")))->leafCount == 1);
        assert(compTrie.find("these", int(strlen("these"))) == nullptr);
        assert(compTrie.find("their", int(strlen("their")))->leafCount == 1);
        assert(compTrie.find("thaw", int(strlen("thaw"))) == nullptr);
        assert(compTrie.find("any", int(strlen("any"))) == nullptr);

        assert(compTrie.search("t", int(strlen("t"))) == make_pair(1, false));
        assert(compTrie.search("th", int(strlen("th"))) == make_pair(2, false));
        assert(compTrie.search("the", int(strlen("the"))) == make_pair(3, true));
        assert(compTrie.search("thei", int(strlen("thei"))) == make_pair(4, false));
        assert(compTrie.search("their", int(strlen("their"))) == make_pair(5, true));

        //cout << "*** after deletion ***" << endl;
        //dump(&compTrie.mRoot, 0);
    }
    // memory & speed test
    {
        int L = 500;
        int N = 10000;
        vector<string> in;

        for (int i = 0; i < N; i++)
            in.push_back(makeRandomString(RandInt32::get() % L + 1));

        PROFILE_START(0);
        {
            CompressedTrie<> tree;
            for (int i = 0; i < N; i++) {
                tree.insert(in[i]);
            }
            if (count(tree.mRoot.children, tree.mRoot.children + 26, nullptr) == 26)
                cerr << "What?" << endl;
        }
        PROFILE_STOP(0);

        PROFILE_START(1);
        {
            CompressedTrieAM<> tree;
            for (int i = 0; i < N; i++) {
                tree.insert(in[i]);
            }
            if (tree.mRoot.children.empty())
                cerr << "What?" << endl;
        }
        PROFILE_STOP(1);
    }

    cout << "OK!" << endl;
}
