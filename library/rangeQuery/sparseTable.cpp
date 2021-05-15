#include <memory.h>
#include <queue>
#include <stack>
#include <algorithm>

using namespace std;

#include "segmentTree.h"
#include "segmentTreeRMQ.h"
#include "sparseTable.h"
#include "sparseTableIndex.h"
#include "sparseTableMin.h"
#include "sparseTableMinIndex.h"

/////////// For Testing ///////////////////////////////////////////////////////

#include <time.h>
#include <cassert>
#include <string>
#include <iostream>
#include "../common/iostreamhelper.h"
#include "../common/rand.h"

void testSparseTable() {
    return; //TODO: if you want to test, make this line a comment.

    cout << "--- Sparse Table ----------------------------------------" << endl;

    cout << "*** RMQ ***" << endl;
    {
        auto sparseTable = makeSparseTable<int>(vector<int>{6, 1, 4, 3, 7, 1}, [](int a, int b) { return min(a, b); }, INT_MAX);
        RMQ<int> rmq(vector<int>{6, 1, 4, 3, 7, 1});

        int ans, ansRMQ;

        ans = sparseTable.query(2, 5);
        cout << ans << endl;

        ansRMQ = rmq.query(2, 5);
        assert(ans == ansRMQ);

        ans = sparseTable.query(0, 3);
        cout << ans << endl;

        ansRMQ = rmq.query(0, 3);
        assert(ans == ansRMQ);

        cout << "OK!" << endl;
    }

    cout << "*** SUM ***" << endl;
    {
        int N = 10000;
        vector<int> inSum(N);
        for (int i = 0; i < N; i++)
            inSum[i] = RandInt32::get() % 65536;

        vector<int> prefixSum(N + 1);
        prefixSum[0] = 0;
        for (int i = 1; i <= N; i++) {
            prefixSum[i] = prefixSum[i - 1] + inSum[i - 1];
        }

        auto sparseTable2 = makeSparseTable<int>(inSum, [](int a, int b) { return a + b; }, 0);

        for (int i = 0; i < 10000; i++) {
            int left = RandInt32::get() % N;
            int right = RandInt32::get() % N;
            if (left > right)
                swap(left, right);

            int ans1 = prefixSum[right + 1] - prefixSum[left];
            int ans2 = sparseTable2.queryNoOverlap(left, right);
            assert(ans1 == ans2);
        }
        cout << "OK!" << endl;
    }

    cout << "-- Segment Tree & Sparse Table Performance Test --------" << endl;
    {
        int N = 1000000;
        int TN = 10;
#ifdef _DEBUG
        N = 10000;
#endif
        vector<int> T(N);
        for (int i = 0; i < N; i++)
            T[i] = RandInt32().get() % 65536;

        vector<pair<int, int>> Q;
        for (int i = 0; i < N; i++) {
            int a = RandInt32::get() % N;
            int b = RandInt32::get() % N;
            Q.push_back({ min(a, b), max(a, b) });
        }

        int res = 0;

        cout << "*** Segment tree ***" << endl;
        res = 0;
        clock_t start = clock();
        auto seg = makeSegmentTree(T, [](int a, int b) { return min(a, b); }, INT_MAX);
        for (int i = 0; i < TN; i++) {
            for (auto& it : Q) {
                res += seg.query(it.first, it.second);
            }
        }
        cout << "elapsed time(" << res << ") : " << double(clock() - start) / CLOCKS_PER_SEC << endl;

        cout << "*** Range Minimum Query (Segment Tree's special version) ***" << endl;
        res = 0;
        start = clock();
        RMQ<int> rmq(T);
        for (int i = 0; i < TN; i++) {
            for (auto& it : Q) {
                res += rmq.query(it.first, it.second);
            }
        }
        cout << "elapsed time(" << res << ") : " << double(clock() - start) / CLOCKS_PER_SEC << endl;

        cout << "*** Sparse Table ***" << endl;
        res = 0;
        start = clock();
        SparseTableMin st(T);
        for (int i = 0; i < TN; i++) {
            for (auto& it : Q) {
                res += st.query(it.first, it.second);
            }
        }
        cout << "elapsed time(" << res << ") : " << double(clock() - start) / CLOCKS_PER_SEC << endl;
    }

    cout << "OK!" << endl;
}
