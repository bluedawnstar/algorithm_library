#pragma once

/*
    1. update
        A[i] = max(A[i], x)   ,  L <= i <= R

    2. query #1
        min(A[L], A[L + 1], ..., A[R])

    3. query #2
        sum(A[L], A[L + 1], ..., A[R])
*/
template <typename T>
struct SegmentTreeBeatsMax {
    static const T INF = numeric_limits<T>::max();

    struct Node {
        T   minValue;
        T   minValue2;

        T   cntMin;
        T   sumValue;

        Node() {
            init();
        }

        void init() {
            minValue = INF;
            minValue2 = INF;

            cntMin = 0;
            sumValue = 0;
        }

        void init(T x) {
            minValue = x;
            minValue2 = INF;

            cntMin = 1;
            sumValue = x;
        }


        void mergeOp(const Node& L, const Node& R) {
            if (L.minValue == R.minValue) {
                minValue = L.minValue;
                minValue2 = min(L.minValue2, R.minValue2);
                cntMin = L.cntMin + R.cntMin;
            } else {
                minValue = min(L.minValue, R.minValue);
                minValue2 = min(max(L.minValue, R.minValue), min(L.minValue2, R.minValue2));
                cntMin = (minValue == L.minValue) ? L.cntMin : R.cntMin;
            }

            sumValue = L.sumValue + R.sumValue;
        }

        bool isBreakCondition(T value) const {
            return value <= minValue;
        }

        bool isTagCondition(T value) const {
            return value < minValue2;
        }
    };

    int       N;                // the size of array
    vector<Node> tree;          // 

    SegmentTreeBeatsMax() {
    }

    void init(int size) {
        N = size;
        tree = vector<Node>(size * 4);
    }

    void build(T value, int n) {
        init(n);
        buildSub(value, 0, n - 1, 1);
    }

    void build(const T arr[], int n) {
        init(n);
        buildSub(arr, 0, n - 1, 1);
    }

    void build(const vector<T>& v) {
        build(&v[0], int(v.size()));
    }


    T querySum(int left, int right) {
        return querySumSub(left, right, 1, 0, N - 1);
    }


    // A[i] = max(A[i], X), inclusive
    void updateMax(int left, int right, T newValue) {
        updateSub(left, right, newValue, 1, 0, N - 1);
    }

    T queryMin(int left, int right) {
        return queryMinSub(left, right, 1, 0, N - 1);
    }


private:
    void pushDown(int node, int nodeLeft, int nodeRight) {
        if (nodeLeft == nodeRight)
            return;

        // left node
        if (tree[node].minValue > tree[node * 2].minValue) {
            tree[node * 2].sumValue += tree[node * 2].cntMin * (tree[node].minValue - tree[node * 2].minValue);
            tree[node * 2].minValue = tree[node].minValue;
        }

        // right node
        if (tree[node].minValue > tree[node * 2 + 1].minValue) {
            tree[node * 2 + 1].sumValue += tree[node * 2 + 1].cntMin * (tree[node].minValue - tree[node * 2 + 1].minValue);
            tree[node * 2 + 1].minValue = tree[node].minValue;
        }
    }

    //---

    // inclusive
    int buildSub(const T arr[], int left, int right, int node) {
        if (left > right)
            return 0;

        if (left == right) {
            tree[node].init(arr[left]);
            return node;
        }

        int mid = left + (right - left) / 2;
        int nodeL = buildSub(arr, left, mid, node * 2);
        int nodeR = buildSub(arr, mid + 1, right, node * 2 + 1);

        tree[node].mergeOp(tree[nodeL], tree[nodeR]);

        return node;
    }

    // inclusive
    int buildSub(T value, int left, int right, int node) {
        if (left > right)
            return 0;

        if (left == right) {
            tree[node].init(value);
            return node;
        }

        int mid = left + (right - left) / 2;
        int nodeL = buildSub(value, left, mid, node * 2);
        int nodeR = buildSub(value, mid + 1, right, node * 2 + 1);

        tree[node].mergeOp(tree[nodeL], tree[nodeR]);

        return node;
    }

    int updateSub(int left, int right, T newValue, int node, int nodeLeft, int nodeRight) {
        if (right < nodeLeft || nodeRight < left || tree[node].isBreakCondition(newValue))
            return node;

        pushDown(node, nodeLeft, nodeRight);
        if (left <= nodeLeft && nodeRight <= right && tree[node].isTagCondition(newValue)) {
            tree[node].sumValue += tree[node].cntMin * (newValue - tree[node].minValue);
            tree[node].minValue = newValue;
            //pushDown(node, nodeLeft, nodeRight);
            return node;
        }

        int mid = nodeLeft + (nodeRight - nodeLeft) / 2;
        int nodeL = updateSub(left, right, newValue, node * 2, nodeLeft, mid);
        int nodeR = updateSub(left, right, newValue, node * 2 + 1, mid + 1, nodeRight);

        tree[node].mergeOp(tree[nodeL], tree[nodeR]);

        return node;
    }

    //---

    T queryMinSub(int left, int right, int node, int nodeLeft, int nodeRight) {
        if (right < nodeLeft || nodeRight < left)
            return INF;

        if (left <= nodeLeft && nodeRight <= right)
            return tree[node].minValue;

        pushDown(node, nodeLeft, nodeRight);

        int mid = nodeLeft + (nodeRight - nodeLeft) / 2;
        auto resL = queryMinSub(left, right, node * 2, nodeLeft, mid);
        auto resR = queryMinSub(left, right, node * 2 + 1, mid + 1, nodeRight);

        return min(resL, resR);
    }

    T querySumSub(int left, int right, int node, int nodeLeft, int nodeRight) {
        if (right < nodeLeft || nodeRight < left)
            return 0;

        if (left <= nodeLeft && nodeRight <= right)
            return tree[node].sumValue;

        pushDown(node, nodeLeft, nodeRight);

        int mid = nodeLeft + (nodeRight - nodeLeft) / 2;
        auto resL = querySumSub(left, right, node * 2, nodeLeft, mid);
        auto resR = querySumSub(left, right, node * 2 + 1, mid + 1, nodeRight);

        return resL + resR;
    }
};