#include <memory.h>
#include <queue>
#include <stack>
#include <unordered_map>
#include <algorithm>

using namespace std;

#include "segmentTreePersistent.h"
#include "segmentTreePersistentLazy.h"

/////////// For Testing ///////////////////////////////////////////////////////

#include <time.h>
#include <cassert>
#include <string>
#include <iostream>
#include "../common/iostreamhelper.h"
#include "../common/profile.h"
#include "../common/rand.h"

#include "segmentTree.h"
#include "segmentTreeRMQ.h"

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

void testSegmentTreePersistent() {
    return; //TODO: if you want to test, make this line a comment.

    cout << "-- Persistent Segment Tree ----------------------------------------" << endl;
    {
        auto segTree = makePersistentSegmentTree(vector<int>{6, 5, 4, 3, 2, 1}, [](int a, int b) { return a + b; });
        auto segTree2 = makePersistentSegmentTree(vector<int>{6, 5, 4, 3, 2, 1}, [](int a, int b) { return min(a, b); }, INT_MAX);
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

        segTree.updateRange(0, 2, 3);
        ans = segTree.query(1, 3);
        //cout << ans << endl;
        assert(ans == 9);

        segTree2.updateRange(0, 2, 2);
        ans = segTree2.query(1, 3);
        //cout << ans << endl;
        assert(ans == 2);

        cout << "*** with history" << endl;

        int history = 0;

        segTree.upgrade(3, 7);
        segTree2.upgradeRange(3, 4, 7);

        ans = segTree.query(history, 1, 3);
        //cout << ans << endl;
        assert(ans == 9);

        segTree.upgrade(2, 10);
        ans = segTree.query(history, 1, 3);
        //cout << ans << endl;
        assert(ans == 9);

        ans = segTree2.query(history, 1, 3);
        //cout << ans << endl;
        assert(ans == 2);

        segTree2.upgrade(2, -10);
        ans = segTree2.query(history, 1, 3);
        //cout << ans << endl;
        assert(ans == 2);

        segTree.upgradeRange(0, 2, 3);
        ans = segTree.query(history, 1, 3);
        //cout << ans << endl;
        assert(ans == 9);

        segTree2.upgradeRange(0, 2, 2);
        ans = segTree2.query(history, 1, 3);
        //cout << ans << endl;
        assert(ans == 2);
    }
    cout << "OK!" << endl;
    {
        int N = 1000;
        vector<int> in(N);

        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % 1000;

        PersistentSegmentTree<int> tree(in, [](int a, int b) { return a + b; }, 0);

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
        int N = 100;
        int T = 100;
        vector<int> in(N);

        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % 1000;

        PersistentSegmentTree<int> tree(in, [](int a, int b) { return a + b; }, 0);

        vector<vector<pair<int, int>>> gt(T, vector<pair<int, int>>(N));    // (sum, index)
        for (int j = 0; j < T; j++) {
            for (int i = 0; i < N; i++) {
                int R = RandInt32::get() % N;
                int sum = sumSlow(in, 0, R);
                gt[j][i].first = sum;
                gt[j][i].second = lowerBoundSlow(in, sum);
            }

            int R = RandInt32::get() % N;
            int newVal = RandInt32::get() % 1000;
            tree.upgrade(R, newVal);
            in[R] = newVal;
        }

        for (int j = 0; j < T; j++) {
            for (int i = 0; i < N; i++) {
                int sum = gt[j][i].first;

                auto ans = tree.lowerBound(j, [sum](int val) { return val >= sum; });
                if (ans != gt[j][i].second) {
                    cerr << "[" << sum << "] ans = " << ans << ", gt = " << gt << endl;
                    ans = tree.lowerBound(j, [sum](int val) { return val >= sum; });
                }
                assert(ans == gt[j][i].second);
            }
        }
    }
    cout << "OK!" << endl;
    {
        int N = 1000;
        vector<int> in(N);

        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % 1000;

        PersistentSegmentTreeLazy<int> tree(in, [](int a, int b) { return a + b; }, 0);

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
        int N = 100;
        int T = 100;
        vector<int> in(N);

        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % 1000;

        PersistentSegmentTreeLazy<int> tree(in, [](int a, int b) { return a + b; }, 0);

        vector<vector<pair<int, int>>> gt(T, vector<pair<int, int>>(N));    // (sum, index)
        for (int j = 0; j < T; j++) {
            for (int i = 0; i < N; i++) {
                int R = RandInt32::get() % N;
                int sum = sumSlow(in, 0, R);
                gt[j][i].first = sum;
                gt[j][i].second = lowerBoundSlow(in, sum);
            }

            int R = RandInt32::get() % N;
            int newVal = RandInt32::get() % 1000;
            tree.upgrade(R, newVal);
            in[R] = newVal;
        }

        for (int j = 0; j < T; j++) {
            for (int i = 0; i < N; i++) {
                int sum = gt[j][i].first;

                auto ans = tree.lowerBound(j, [sum](int val) { return val >= sum; });
                if (ans != gt[j][i].second) {
                    cerr << "[" << sum << "] ans = " << ans << ", gt = " << gt << endl;
                    ans = tree.lowerBound(j, [sum](int val) { return val >= sum; });
                }
                assert(ans == gt[j][i].second);
            }
        }
    }
    cout << "OK!" << endl;
    cout << "-- Persistent Segment Tree Performance Test -----------------------" << endl;
    cout << "*** Persistent segment tree vs RMQ" << endl;
    {
        int N = 1000000;
#ifdef _DEBUG
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
            auto seg = makePersistentSegmentTree(T, [](int a, int b) { return min(a, b); }, INT_MAX);
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
    cout << "*** Persistent segment tree vs persistent segment tree with lazy propagation" << endl;
    {
        int T = 100000;
        int N = 1000000;
#ifdef _DEBUG
        T = 1000;
        N = 10000;
#endif
        vector<int> in(N);

        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % 1000;

        auto segTree = makePersistentSegmentTree(in, [](int a, int b) { return a + b; });
        auto segTreeLazy = makePersistentSegmentTreeLazy(in, [](int a, int b) { return a + b; }, [](int a, int n) { return a * n; });

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
                if (a != b) {
                    cout << "FAIL in query()" << endl;
                    assert(a == b);
                }
            } else {
                int L = get<1>(q);
                int R = get<2>(q);
                int x = get<3>(q);
                int a = segTree.updateRange(L, R, x);
                int b = segTreeLazy.updateRange(L, R, x);
                if (a != b) {
                    cout << "FAIL in update()" << endl;
                    assert(a == b);
                }
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
                segTree.updateRange(L, R, x);
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
                segTreeLazy.updateRange(L, R, x);
            }
        }
        PROFILE_STOP(1);
    }
    cout << "*** with history" << endl;
    {
        int T = 100000;
        int N = 1000000;
#ifdef _DEBUG
        T = 1000;
        N = 10000;
#endif
        vector<int> in(N);

        for (int i = 0; i < N; i++)
            in[i] = RandInt32::get() % 1000;

        auto segTree = makePersistentSegmentTree(in, [](int a, int b) { return a + b; });
        auto segTreeLazy = makePersistentSegmentTreeLazy(in, [](int a, int b) { return a + b; }, [](int a, int n) { return a * n; });

        int history = 0;
        segTree.upgradeRange(3, 4, 7);
        segTreeLazy.upgradeRange(3, 4, 7);

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

                int a = segTree.query(history, L, R);
                int b = segTreeLazy.query(history, L, R);
                if (a != b) {
                    cout << "FAIL!" << endl;
                    assert(a == b);
                }
            } else {
                int L = get<1>(q);
                int R = get<2>(q);
                int x = get<3>(q);
                int a = segTree.updateRange(history, L, R, x);
                int b = segTreeLazy.updateRange(history, L, R, x);
                if (a != b) {
                    cout << "FAIL in update()" << endl;
                    assert(a == b);
                }
            }
        }
        cout << "OK!" << endl;

        PROFILE_START(0);
        for (auto& q : Q) {
            if (get<0>(q)) {
                int L = get<1>(q);
                int R = get<2>(q);
                if (segTree.query(history, L, R) == INT_MAX)
                    cout << "It'll Never be shown!" << endl;
            } else {
                int L = get<1>(q);
                int R = get<2>(q);
                int x = get<3>(q);
                segTree.updateRange(history, L, R, x);
            }
        }
        PROFILE_STOP(0);

        PROFILE_START(1);
        for (auto& q : Q) {
            if (get<0>(q)) {
                int L = get<1>(q);
                int R = get<2>(q);
                if (segTreeLazy.query(history, L, R) == INT_MAX)
                    cout << "It'll Never be shown!" << endl;
            } else {
                int L = get<1>(q);
                int R = get<2>(q);
                int x = get<3>(q);
                segTreeLazy.updateRange(history, L, R, x);
            }
        }
        PROFILE_STOP(1);
    }

    cout << "OK!" << endl;
}
