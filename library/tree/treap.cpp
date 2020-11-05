#include <cassert>
#include <chrono>
#include <random>
#include <tuple>
#include <vector>

using namespace std;

#include "treap.h"
#include "treapForest.h"

/////////// For Testing ///////////////////////////////////////////////////////

#include <time.h>
#include <cassert>
#include <string>
#include <numeric>
#include <set>
#include <vector>
#include <iostream>
#include "../common/iostreamhelper.h"
#include "../common/profile.h"

#include "redBlackTree.h"
#include "splayTree.h"

static void checkSearch(RBTree<int>& rbt, vector<int>& in) {
    for (int i = 0; i < int(in.size()); i++) {
        int x = in[i];
        assert(rbt.find(x)->value == x);
    }
}

static void checkIndex(RBTree<int>& rbt, vector<int>& in) {
    assert(rbt.size() == int(in.size()));
    for (int i = 0; i < int(in.size()); i++) {
        assert(rbt[i]->value == in[i]);
        assert(rbt.indexOf(rbt[i]) == i);
    }
}

static void checkSearch(Treap<int>& tr, vector<int>& in) {
    for (int i = 0; i < int(in.size()); i++) {
        int x = in[i];
        assert(tr.find(x)->value == x);
    }
}

static void checkIndex(Treap<int>& tr, vector<int>& in) {
    assert((tr.tree != nullptr ? tr.tree->cnt : 0) == tr.size());
    assert(tr.size() == int(in.size()));
    for (int i = 0; i < int(in.size()); i++) {
        assert(tr[i]->value == in[i]);
        assert(tr.indexOf(tr[i]) == i);
    }
}

static void checkSearch(TreapForest<int>& tr, TreapForest<int>::Node* root, vector<int>& in) {
    for (int i = 0; i < int(in.size()); i++) {
        int x = in[i];
        assert(tr.find(root, x)->value == x);
    }
}

static void checkIndex(TreapForest<int>& tr, TreapForest<int>::Node* root, vector<int>& in) {
    assert(tr.size(root) == int(in.size()));
    for (int i = 0; i < int(in.size()); i++) {
        assert(tr.kth(root, i)->value == in[i]);
    }
}

void testTreap() {
    return; //TODO: if you want to test, make this line a comment.

    cout << "--- Treap ----------------------------------" << endl;

    srand(unsigned(time(nullptr)));
    {
        RBTree<int> rbt;
        Treap<int> tr;
        TreapForest<int> trForest;

        vector<int> in(1000);
        iota(in.begin(), in.end(), 0);

        TreapForest<int>::Node* trForestRoot = nullptr;
        {
            vector<int> t(in);
            random_shuffle(t.begin(), t.end());
            for (int i = 0; i < int(in.size()); i++) {
                auto p1 = rbt.insert(t[i]);
                if (!p1.second)
                    cerr << "It'll never be shown!" << endl;
                assert(p1.first->value == t[i] && p1.second);

                auto p2 = tr.insert(t[i]);
                if (!p2)
                    cerr << "It'll never be shown!" << endl;
                assert(p2 && p2->value == t[i]);

                TreapForest<int>::Node* p3 = nullptr;
                trForestRoot = trForest.insert(trForestRoot, t[i], &p3);
                if (!p3)
                    cerr << "It'll never be shown!" << endl;
                assert(p3 && p3->value == t[i]);
            }
            checkSearch(rbt, in);
            checkIndex(rbt, in);
            checkSearch(tr, in);
            checkIndex(tr, in);
            checkSearch(trForest, trForestRoot, in);
            checkIndex(trForest, trForestRoot, in);
        }
        {
            vector<int> t(in), org(in);

            random_shuffle(t.begin(), t.end());
            while (!rbt.empty()) {
                int x = t.back();
                t.pop_back();
                org.erase(find(org.begin(), org.end(), x));

                bool b1 = rbt.erase(x);
                if (!b1)
                    cerr << "It'll never be shown!" << endl;
                assert(b1);
                assert(rbt.size() == int(t.size()));
                checkSearch(rbt, org);
                checkIndex(rbt, org);

                bool b2 = tr.erase(x);
                if (!b2)
                    cerr << "It'll never be shown!" << endl;
                assert(b2);
                assert(tr.size() == int(t.size()));
                checkSearch(tr, org);
                checkIndex(tr, org);

                trForestRoot = trForest.erase(trForestRoot, x);
                assert(trForest.size(trForestRoot) == int(t.size()));
                checkSearch(trForest, trForestRoot, org);
                checkIndex(trForest, trForestRoot, org);
            }
        }
    }
    {
        RBTree<int> rbt;
        Treap<int> tr;
        TreapForest<int> trForest;

        vector<int> in(1000);
        iota(in.begin(), in.end(), 0);

        TreapForest<int>::Node* trForestRoot = nullptr;
        {
            vector<int> t(in);
            random_shuffle(t.begin(), t.end());

            vector<int> sorted(in);
            sort(sorted.begin(), sorted.end());

            tr.build(sorted);
            trForestRoot = trForest.build(sorted);
            for (int i = 0; i < int(in.size()); i++) {
                auto p1 = rbt.insert(t[i]);
                if (!p1.second)
                    cerr << "It'll never be shown!" << endl;
                assert(p1.first->value == t[i] && p1.second);
            }
            checkSearch(rbt, in);
            checkIndex(rbt, in);
            checkSearch(tr, in);
            checkIndex(tr, in);
            checkSearch(trForest, trForestRoot, in);
            checkIndex(trForest, trForestRoot, in);
        }
        {
            vector<int> t(in), org(in);

            random_shuffle(t.begin(), t.end());
            while (!rbt.empty()) {
                int x = t.back();
                t.pop_back();
                org.erase(find(org.begin(), org.end(), x));

                bool b1 = rbt.erase(x);
                if (!b1)
                    cerr << "It'll never be shown!" << endl;
                assert(b1);
                assert(rbt.size() == int(t.size()));
                checkSearch(rbt, org);
                checkIndex(rbt, org);

                bool b2 = tr.erase(x);
                if (!b2)
                    cerr << "It'll never be shown!" << endl;
                assert(b2);
                assert(tr.size() == int(t.size()));
                checkSearch(tr, org);
                checkIndex(tr, org);

                trForestRoot = trForest.erase(trForestRoot, x);
                assert(trForest.size(trForestRoot) == int(t.size()));
                checkSearch(trForest, trForestRoot, org);
                checkIndex(trForest, trForestRoot, org);
            }
        }
    }
    cout << "OK!" << endl;

    cout << "*** Speed test RBTree vs SplayTree vs Treap ***" << endl;
    {
        RBTree<int> rbt;
        SplayTree<int> spt;
        Treap<int> tr;

#ifdef _DEBUG
        vector<int> in(300000);
#else
        vector<int> in(3000000);
#endif
        iota(in.begin(), in.end(), 0);

        cout << "1) Insertion test" << endl;
        {
            vector<int> t(in);
            random_shuffle(t.begin(), t.end());

            PROFILE_START(0);
            for (int i = 0; i < int(t.size()); i++) {
                auto p = rbt.insert(t[i]);
                if (!p.second)
                    cerr << "It'll never be shown!" << endl;
            }
            PROFILE_STOP(0);

            PROFILE_START(1);
            for (int i = 0; i < int(t.size()); i++) {
                auto p = spt.insert(t[i]);
                if (!p.second)
                    cerr << "It'll never be shown!" << endl;
            }
            PROFILE_STOP(1);

            PROFILE_START(2);
            for (int i = 0; i < int(t.size()); i++) {
                auto p = tr.insert(t[i]);
                if (!p)
                    cerr << "It'll never be shown!" << endl;
            }
            PROFILE_STOP(2);
        }
        cout << "2) Find test" << endl;
        {
            vector<int> t(in);
            random_shuffle(t.begin(), t.end());

            PROFILE_START(0);
            for (int i = 0; i < int(t.size()); i++) {
                if (rbt.find(t[i])->value != t[i])
                    cerr << "It'll never be shown!" << endl;
            }
            PROFILE_STOP(0);

            PROFILE_START(1);
            for (int i = 0; i < int(t.size()); i++) {
                if (spt.find(t[i])->value != t[i])
                    cerr << "It'll never be shown!" << endl;
            }
            PROFILE_STOP(1);

            PROFILE_START(2);
            for (int i = 0; i < int(t.size()); i++) {
                if (tr.find(t[i])->value != t[i])
                    cerr << "It'll never be shown!" << endl;
            }
            PROFILE_STOP(2);
        }
        cout << "3) Erase test" << endl;
        {
            vector<int> t(in);
            random_shuffle(t.begin(), t.end());

            PROFILE_START(0);
            for (int i = 0; i < int(t.size()); i++) {
                if (!rbt.erase(t[i]))
                    cerr << "It'll never be shown!" << endl;
            }
            PROFILE_STOP(0);

            PROFILE_START(1);
            for (int i = 0; i < int(t.size()); i++) {
                if (!spt.erase(t[i]))
                    cerr << "It'll never be shown!" << endl;
            }
            PROFILE_STOP(1);

            PROFILE_START(2);
            for (int i = 0; i < int(t.size()); i++) {
                if (!tr.erase(t[i]))
                    cerr << "It'll never be shown!" << endl;
            }
            PROFILE_STOP(2);
        }
    }
    cout << "OK!" << endl;
}
