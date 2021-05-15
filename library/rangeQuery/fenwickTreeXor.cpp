using namespace std;

#include "fenwickTreeXor.h"

/////////// For Testing ///////////////////////////////////////////////////////

#include <memory.h>
#include <queue>
#include <stack>
#include <algorithm>
#include <time.h>
#include <cassert>
#include <string>
#include <iostream>
#include "../common/iostreamhelper.h"
#include "../common/rand.h"

static int xorSlow(vector<int>& v, int L, int R) {
    int res = 0;
    while (L <= R)
        res ^= v[L++];
    return res;
}

void testFenwickTreeXor() {
    //return; //TODO: if you want to test, make this line a comment.

    cout << "-- XOR FenwickTree (Binary Indexed Tree) -------------------" << endl;
    {
        int N = 1000;
        int T = 1000;
        FenwickTreeXor<int> xorTree(N);
        vector<int> in(N);
        for (int i = 0; i < N; i++) {
            in[i] = RandInt32::get() % 65536;
            xorTree.add(i, in[i]);
        }
        for (int i = 0; i < T; i++) {
            int L = RandInt32::get() % N;
            int R = RandInt32::get() % N;
            if (L > R)
                swap(L, R);
            assert(xorTree.query(L) == xorSlow(in, 0, L));
            assert(xorTree.query(R) == xorSlow(in, 0, R));
            assert(xorTree.queryRange(0, L) == xorSlow(in, 0, L));
            assert(xorTree.queryRange(0, R) == xorSlow(in, 0, R));
            assert(xorTree.get(L) == in[L]);
            assert(xorTree.get(R) == in[R]);
            assert(xorTree.queryRange(L, R) == xorSlow(in, L, R));
            in[R] = RandInt32::get() % 65536;
            xorTree.set(R, in[R]);
            assert(xorTree.get(R) == in[R]);
        }
    }
    cout << "OK!" << endl;
    {
        int N = 1000;
        int T = 1000;

        vector<int> in(N);
        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % 65536;

        FenwickTreeXor<int> xorTree(in);
        {
            FenwickTreeXor<int> ft;
            ft.init(N);
            for (int i = 0; i < N; i++)
                ft.add(i, in[i]);

            if (xorTree.tree != ft.tree) {
                cerr << "Mismatched tree structure at " << __LINE__ << " in " << __FILE__ << endl;
            }
            assert(xorTree.tree == ft.tree);
        }
        for (int i = 0; i < T; i++) {
            int L = RandInt32::get() % N;
            int R = RandInt32::get() % N;
            if (L > R)
                swap(L, R);
            assert(xorTree.query(L) == xorSlow(in, 0, L));
            assert(xorTree.query(R) == xorSlow(in, 0, R));
            assert(xorTree.queryRange(0, L) == xorSlow(in, 0, L));
            assert(xorTree.queryRange(0, R) == xorSlow(in, 0, R));
            assert(xorTree.get(L) == in[L]);
            assert(xorTree.get(R) == in[R]);
            assert(xorTree.queryRange(L, R) == xorSlow(in, L, R));
            in[R] = RandInt32::get() % 65536;
            xorTree.set(R, in[R]);
            assert(xorTree.get(R) == in[R]);
        }
    }
    cout << "OK!" << endl;
}
