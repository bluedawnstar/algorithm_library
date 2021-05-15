#pragma once

#include <vector>

//--------- XOR Fenwick Tree (Binary Indexed Tree) ----------------------------

// for xor from 0 to pos
template <typename T>
struct FenwickTreeXor {
    vector<T> tree;

    FenwickTreeXor() {
    }

    explicit FenwickTreeXor(int n) : tree(n + 1) {
    }

    FenwickTreeXor(T value, int n) {
        build(value, n);
    }

    FenwickTreeXor(const T arr[], int n) {
        build(arr, n);
    }

    FenwickTreeXor(const vector<T>& v) {
        build(v);
    }


    void init(int n) {
        tree = vector<T>(n + 1);
    }


    // O(N)
    void build(T value, int n) {
        tree = vector<T>(n + 1, value);
        tree[0] = T(0);
        for (int step = 2; step <= n; step <<= 1) {
            for (int i = step >> 1, j = step; j <= n; i += step, j += step)
                tree[j] ^= tree[i];
        }
    }

    // O(N)
    void build(const T arr[], int n) {
        tree.clear();
        tree.push_back(T(0));
        tree.insert(tree.end(), arr, arr + n);
        for (int step = 2; step <= n; step <<= 1) {
            for (int i = step >> 1, j = step; j <= n; i += step, j += step)
                tree[j] ^= tree[i];
        }
    }

    // O(N)
    void build(const vector<T>& v) {
        build(&v[0], int(v.size()));
    }


    void add(int pos, T val) {
        pos++;

        while (pos < int(tree.size())) {
            tree[pos] ^= val;
            pos += pos & -pos;      // add lowest bit
        }
    }

    // xor from 0 to pos
    T query(int pos) const {
        pos++;

        T res = 0;
        while (pos > 0) {
            res ^= tree[pos];
            pos &= pos - 1;         // clear lowest bit
        }

        return res;
    }

    // inclusive
    T queryRange(int left, int right) const {
        T res = query(right);
        if (left > 0)
            res ^= query(left - 1);
        return res;
    }


    T get(int pos) const {
        T res = tree[pos + 1];
        if (pos > 0) {
            int lca = pos & (pos + 1);
            for (; pos != lca; pos &= pos - 1)
                res ^= tree[pos];
        }

        return res;
    }

    void set(int pos, T val) {
        add(pos, val ^ get(pos));
    }
};
