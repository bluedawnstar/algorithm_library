#include <memory.h>
#include <queue>
#include <stack>
#include <algorithm>

using namespace std;

#include "segmentTree.h"
#include "segmentTreeLazy.h"
#include "segmentTreeRMQ.h"

/////////// For Testing ///////////////////////////////////////////////////////

#include <time.h>
#include <cassert>
#include <string>
#include <iostream>
#include "../common/iostreamhelper.h"
#include "../common/profile.h"
#include "../common/rand.h"

static int getMin(const vector<int>& v, int L, int R) {
    int res = v[L];
    while (++L <= R) {
        if (res > v[L])
            res = v[L];
    }
    return res;
}

static int sumSlow(vector<int>& v, int L, int R) {
    int res = 0;
    while (L <= R)
        res += v[L++];
    return res;
}

static int lowerBoundSlow(vector<int>& v, int k) {
    int res = 0;
    for (int i = 0; i < int(v.size()); i++) {
        res += v[i];
        if (res >= k)
            return i;
    }
    return int(v.size());
}

static void testRMQ(int N, int T) {
    vector<int> value(N);
    for (int i = 0; i < N; i++)
        value[i] = RandInt32::get();

    vector<pair<int, int>> query(T);
    for (int i = 0; i < T; i++) {
        int L = RandInt32::get() % N;
        int R = RandInt32::get() % N;
        if (L > R)
            swap(L, R);
        query[i].first = L;
        query[i].second = R;
    }

    RMQ<int> rmq(value);
    for (int i = 0; i < T; i++) {
        int ans = rmq.query(query[i].first, query[i].second);
        int gt = getMin(value, query[i].first, query[i].second);
        if (ans != gt)
            cout << "Mismatch at " << i << ": " << ans << ", " << gt << endl;
        assert(ans == gt);
    }
}

void testSegmentTree() {
    //return; //TODO: if you want to test, make this line a comment.

    cout << "--- Segment Tree -------------------------------------" << endl;
    {
        auto segTree = makeSegmentTree(vector<int>{6, 5, 4, 3, 2, 1}, [](int a, int b) { return a + b; });
        auto segTree2 = makeSegmentTree(vector<int>{6, 5, 4, 3, 2, 1}, [](int a, int b) { return min(a, b); }, INT_MAX);
        RMQ<int> rmq(vector<int>{6, 5, 4, 3, 2, 1});

        int ans, ansRMQ;

        ans = segTree.query(1, 3);
        //cout << ans << endl;
        assert(ans == 12);

        segTree.update(2, 10);
        ans = segTree.query(1, 3);
        //cout << ans << endl;
        assert(ans == 18);

        ans = segTree2.query(1, 3);
        //cout << ans << endl;
        assert(ans == 3);

        ansRMQ = rmq.query(1, 3);
        assert(ans == ansRMQ);

        segTree2.update(2, -10);
        ans = segTree2.query(1, 3);
        //cout << ans << endl;
        assert(ans == -10);

        rmq.update(2, -10);
        ansRMQ = rmq.query(1, 3);
        assert(ans == ansRMQ);

        segTree.update(0, 2, 3);
        ans = segTree.query(1, 3);
        //cout << ans << endl;
        assert(ans == 9);

        segTree2.update(0, 2, 2);
        ans = segTree2.query(1, 3);
        //cout << ans << endl;
        assert(ans == 2);
    }
    testRMQ(1, 1);
    testRMQ(2, 2);
    testRMQ(1024, 1000);
    testRMQ(1024 + 1, 1000);
    testRMQ(1024 - 1, 1000);
    cout << "OK!" << endl;
    {
        int N = 1000;
        vector<int> in(N);

        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % 1000;

        SegmentTree<int> tree(in, [](int a, int b) { return a + b; }, 0);

        for (int i = 0; i < N; i++) {
            int R = RandInt32::get() % N;
            int sum = sumSlow(in, 0, R);

            auto ans = tree.lowerBound([sum](int val) { return val >= sum; });
            int gt = lowerBoundSlow(in, sum);
            if (ans != gt) {
                cerr << "[" << sum << "] ans = " << ans << ", gt = " << gt << endl;
                ans = tree.lowerBound([sum](int val) { return val >= sum; });
            }
            assert(ans == gt);
        }
    }
    cout << "OK!" << endl;
    {
        int N = 1000;
        vector<int> in(N);

        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % 1000;

        SegmentTreeLazy<int> tree(in, [](int a, int b) { return a + b; }, 0);

        for (int i = 0; i < N; i++) {
            int R = RandInt32::get() % N;
            int sum = sumSlow(in, 0, R);

            auto ans = tree.lowerBound([sum](int val) { return val >= sum; });
            int gt = lowerBoundSlow(in, sum);
            if (ans.second != gt) {
                cerr << "[" << sum << "] ans = " << ans << ", gt = " << gt << endl;
                ans = tree.lowerBound([sum](int val) { return val >= sum; });
            }
            assert(ans.second == gt);
        }
    }
    cout << "OK!" << endl;
    cout << "-- Segment Tree Performance Test -----------------------" << endl;
    cout << "*** Segment tree vs RMQ" << endl;
    {
        int N = 1000000;
#if _DEBUG
        N = 10000;
#endif

        vector<int> T(N);
        for (int i = 0; i < N; i++)
            T[i] = RandInt32::get() % 65536;

        vector<pair<int, int>> Q;
        for (int i = 0; i < N; i++) {
            int a = RandInt32::get() % N;
            int b = RandInt32::get() % N;
            Q.push_back({ min(a, b), max(a, b) });
        }

        PROFILE_START(0);
        {
            int res = 0;
            auto seg = makeSegmentTree(T, [](int a, int b) { return min(a, b); }, INT_MAX);
            for (int i = 0; i < 10; i++) {
                for (auto& it : Q) {
                    res += seg.query(it.first, it.second);
                }
            }
            cout << "result = " << res << endl;
        }
        PROFILE_STOP(0);

        PROFILE_START(1);
        {
            int res = 0;
            RMQ<int> rmq(T);
            for (int i = 0; i < 10; i++) {
                for (auto& it : Q) {
                    res += rmq.query(it.first, it.second);
                }
            }
            cout << "result = " << res << endl;
        }
        PROFILE_STOP(1);
    }
    cout << "*** Segment tree vs segment tree with lazy propagation" << endl;
    {
        int T = 100000;
        int N = 1000000;
#ifdef _DEBUG
        T = 1000;
        N = 10000;
#endif
        vector<int> in(N);

        auto segTree = makeSegmentTree(in, [](int a, int b) { return a + b; });
        auto segTreeLazy = makeSegmentTreeLazy(in, [](int a, int b) { return a + b; }, [](int a, int n) { return a * n; });

        vector<tuple<int, int, int, int>> Q;
        for (int i = 0; i < T; i++) {
            if (RandInt32::get() % 2) {
                int L = RandInt32::get() % int(in.size());
                int R = RandInt32::get() % int(in.size());
                if (L > R)
                    swap(L, R);
                Q.emplace_back(1, L, R, 0);
            } else {
                int L = RandInt32::get() % int(in.size());
                int R = RandInt32::get() % int(in.size());
                int x = RandInt32::get() % 100;
                if (L > R)
                    swap(L, R);
                Q.emplace_back(0, L, R, x);
            }
        }

        for (auto& q : Q) {
            if (get<0>(q)) {
                int L = get<1>(q);
                int R = get<2>(q);

                int a = segTree.query(L, R);
                int b = segTreeLazy.query(L, R);
                if (a != b)
                    cout << "It'll Never be shown!" << endl;
            } else {
                int L = get<1>(q);
                int R = get<2>(q);
                int x = get<3>(q);
                segTree.update(L, R, x);
                segTreeLazy.update(L, R, x);
            }
        }
        cout << "OK!" << endl;

        PROFILE_START(0);
        for (auto& q : Q) {
            if (get<0>(q)) {
                int L = get<1>(q);
                int R = get<2>(q);
                if (segTree.query(L, R) == INT_MAX)
                    cout << "It'll Never be shown!" << endl;
            } else {
                int L = get<1>(q);
                int R = get<2>(q);
                int x = get<3>(q);
                segTree.update(L, R, x);
            }
        }
        PROFILE_STOP(0);

        PROFILE_START(1);
        for (auto& q : Q) {
            if (get<0>(q)) {
                int L = get<1>(q);
                int R = get<2>(q);
                if (segTreeLazy.query(L, R) == INT_MAX)
                    cout << "It'll Never be shown!" << endl;
            } else {
                int L = get<1>(q);
                int R = get<2>(q);
                int x = get<3>(q);
                segTreeLazy.update(L, R, x);
            }
        }
        PROFILE_STOP(1);
    }

    cout << "OK!" << endl;
}
