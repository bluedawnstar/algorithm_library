#include <memory.h>
#include <vector>
#include <queue>
#include <stack>
#include <functional>
#include <algorithm>

using namespace std;

#include "sqrtTree.h"

/////////// For Testing ///////////////////////////////////////////////////////

#include <time.h>
#include <cassert>
#include <string>
#include <iostream>
#include "../common/iostreamhelper.h"
#include "../common/profile.h"
#include "../common/rand.h"

#include "segmentTree.h"
#include "segmentTreeCompact.h"
#include "sparseTableDisjoint.h"
#include "sparseTable.h"

static const int MOD = 1000000007;

static int sum(const vector<int>& A, int L, int R) {
    int res = 0;
    while (L <= R)
        res += A[L++];
    return res;
}

static int mult(const vector<int>& A, int L, int R, int mod) {
    int res = 1;
    while (L <= R)
        res = 1ll * res * A[L++] % mod;
    return int(res);
}

void testSqrtTree() {
    //return; //TODO: if you want to test, make this line a comment.

    cout << "--- Sqrt-Tree ------------------------" << endl;
    // Sum
    {
        int N = 10000;
        int T = 1000;

        vector<int> in(N);
        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % 100;

        auto tree = makeSqrtTree<int>(in, [](int a, int b) { return a + b; }, 0);

        for (int i = 0; i < T; i++) {
            int left = RandInt32::get() % N;
            int right = RandInt32::get() % N;
            if (left > right)
                swap(left, right);

            {
                int ans1 = sum(in, left, right);
                int ans2 = tree.query(left, right);
                assert(ans1 == ans2);
            }
            {
                int idx = left + (right - left) / 2;
                int val = RandInt32::get() % 100;

                in[idx] = val;
                tree.update(idx, val);
                int ans1 = sum(in, left, right);
                int ans2 = tree.query(left, right);
                assert(ans1 == ans2);
            }
        }
    }
    // Multiplication
    {
        int N = 10000;
        int T = 1000;

        vector<int> in(N);
        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % MOD;

        auto tree = makeSqrtTree<int>(in, [](int a, int b) { return int(1ll * a * b % MOD); }, 1);

        for (int i = 0; i < T; i++) {
            int left = RandInt32::get() % N;
            int right = RandInt32::get() % N;
            if (left > right)
                swap(left, right);

            {
                int ans1 = mult(in, left, right, MOD);
                int ans2 = tree.query(left, right);
                assert(ans1 == ans2);
            }
            {
                int idx = left + (right - left) / 2;
                int val = RandInt32::get() % MOD;

                in[idx] = val;
                tree.update(idx, val);
                int ans1 = mult(in, left, right, MOD);
                int ans2 = tree.query(left, right);
                assert(ans1 == ans2);
            }
        }
    }
    cout << "*** Speed Test for Query ***" << endl;
    // Sparse Table ~ Disjoint Sparse Table > Sqrt Tree >> Compact Segment Tree (x3) >>> Segment Tree (x15)
    {
        int N = 1'000'000;
        int T = 1'000'000;
#ifdef _DEBUG
        N = 1'000;
        T = 1'000;
#endif

        vector<int> in(N);
        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get();

        vector<pair<int, int>> Q;
        for (int i = 0; i < T; i++) {
            int a = RandInt32::get() % N;
            int b = RandInt32::get() % N;
            Q.push_back({ min(a, b), max(a, b) });
        }

        cout << "* Segment Tree" << endl;
        PROFILE_START(0);
        {
            int res = 0;
            auto seg = makeSegmentTree(in, [](int a, int b) { return min(a, b); }, INT_MAX);
            for (int i = 0; i < 10; i++) {
                for (auto& it : Q) {
                    res += seg.query(it.first, it.second);
                }
            }
            cout << "result = " << res << endl;
        }
        PROFILE_STOP(0);

        cout << "* Compact Segment Tree" << endl;
        PROFILE_START(1);
        {
            int res = 0;
            auto seg = makeCompactSegmentTree(in, [](int a, int b) { return min(a, b); }, INT_MAX);
            for (int i = 0; i < 10; i++) {
                for (auto& it : Q) {
                    res += seg.query(it.first, it.second);
                }
            }
            cout << "result = " << res << endl;
        }
        PROFILE_STOP(1);

        cout << "* Disjoint Sparse Table" << endl;
        PROFILE_START(2);
        {
            int res = 0;
            auto seg = makeDisjointSparseTable(in, [](int a, int b) { return min(a, b); }, INT_MAX);
            for (int i = 0; i < 10; i++) {
                for (auto& it : Q) {
                    res += seg.query(it.first, it.second);
                }
            }
            cout << "result = " << res << endl;
        }
        PROFILE_STOP(2);

        cout << "* Sparse Table" << endl;
        PROFILE_START(3);
        {
            int res = 0;
            auto seg = makeSparseTable(in, [](int a, int b) { return min(a, b); }, INT_MAX);
            for (int i = 0; i < 10; i++) {
                for (auto& it : Q) {
                    res += seg.query(it.first, it.second);
                }
            }
            cout << "result = " << res << endl;
        }
        PROFILE_STOP(3);

        cout << "* Sqrt-Tree" << endl;
        PROFILE_START(4);
        {
            int res = 0;
            auto tree = makeSqrtTree(in, [](int a, int b) { return min(a, b); }, INT_MAX);
            for (int i = 0; i < 10; i++) {
                for (auto& it : Q) {
                    res += tree.query(it.first, it.second);
                }
            }
            cout << "result = " << res << endl;
        }
        PROFILE_STOP(4);
    }
    cout << "*** Speed Test for Update & Query ***" << endl;
    {
        int N = 100'000;
        int T = 100'000;
#ifdef _DEBUG
        N = 1'000;
        T = 1'000;
#endif

        vector<int> in(N);
        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get();

        vector<pair<int, int>> Q;
        for (int i = 0; i < T; i++) {
            int a = RandInt32::get() % N;
            int b = RandInt32::get() % N;
            Q.emplace_back(a, b);
        }

        cout << "* Segment Tree" << endl;
        PROFILE_START(0);
        {
            int res = 0;
            auto seg = makeSegmentTree(in, [](int a, int b) { return min(a, b); }, INT_MAX);
            for (int i = 0; i < 10; i++) {
                for (auto& it : Q) {
                    seg.update(it.first, it.second);
                }
            }
            cout << "result = " << seg.query(0, N - 1) << endl;
        }
        PROFILE_STOP(0);

        cout << "* Compact Segment Tree" << endl;
        PROFILE_START(1);
        {
            int res = 0;
            auto seg = makeCompactSegmentTree(in, [](int a, int b) { return min(a, b); }, INT_MAX);
            for (int i = 0; i < 10; i++) {
                for (auto& it : Q) {
                    seg.update(it.first, it.second);
                }
            }
            cout << "result = " << seg.query(0, N - 1) << endl;
        }
        PROFILE_STOP(1);

        cout << "* Sqrt-Tree" << endl;
        PROFILE_START(2);
        {
            int res = 0;
            auto tree = makeSqrtTree(in, [](int a, int b) { return min(a, b); }, INT_MAX);
            for (int i = 0; i < 10; i++) {
                for (auto& it : Q) {
                    tree.update(it.first, it.second);
                }
            }
            cout << "result = " << tree.query(0, N - 1) << endl;
        }
        PROFILE_STOP(2);
    }

    cout << "OK!" << endl;
}
