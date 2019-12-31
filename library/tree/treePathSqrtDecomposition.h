#pragma once

#ifndef __GNUC__
#include <intrin.h>
#endif
#include <immintrin.h>

#define USE_LAZY_REBUILDING

template <typename T, typename MergeOp = function<T(T, T)>, typename BlockOp = function<T(T, int)>>
struct TreePathSqrtDecomposition {
    int                 N;          // the number of vertex
    int                 logN;       // log2(N - 1) + 2

    vector<vector<int>> edges;      // edges (vertex number)
    vector<vector<int>> P;          // P[0][n] points to the parent
                                    // parent & acestors

    vector<int>         level;      // depth (root is 0)
    vector<int>         treeSize;   // call dfsSize() to calculate tree size

    //---

    int         root;                   // 
    int         sqrtN;                  // 
    int         branchN;                // 
    vector<int> branchSize;             // 
    vector<int> branchHead;             // 
    vector<int> branchTail;             // 
    vector<int> nodeToBranch;           // 

    enum LazyT {
        lzNone,
        lzSet,
        lzAdd
    };

    vector<T>       values;             // all values of nodes
    vector<T>       branchValues;       // 
    vector<vector<int>> branchNodes;    // 
    vector<LazyT>   branchLazy;         // 
    vector<T>       branchLazyValues;   // 
#ifdef USE_LAZY_REBUILDING
    vector<bool>    dirty;              // rebuild required
#endif
    T               defaultValue;       // 
    MergeOp         mergeOp;            // 
    BlockOp         blockOp;            // 

    TreePathSqrtDecomposition(const MergeOp& mop, const BlockOp& bop, T dfltValue = T())
        : N(0), logN(0), mergeOp(mop), blockOp(bop), defaultValue(dfltValue)  {
    }

    explicit TreePathSqrtDecomposition(int N, const MergeOp& mop, const BlockOp& bop, T dfltValue = T(), int logN = 0)
        : mergeOp(mop), blockOp(bop), defaultValue(dfltValue) {
        init(N, logN);
    }

    void init(int N, int logN = 0) {
        if (logN <= 0) {
#ifndef __GNUC__
            logN = _lzcnt_u32(1u) - _lzcnt_u32((unsigned int)(N - 1)) + 2;
#else
            logN = __builtin_clz(1u) - __builtin_clz((unsigned int)(N - 1)) + 2;
#endif
        }

        this->N = N;
        this->logN = logN;

        edges = vector<vector<int>>(N);
        P = vector<vector<int>>(logN, vector<int>(N));
        level.assign(N, 0);
        treeSize.assign(N, 0);

        sqrtN = int(sqrt(N));
        nodeToBranch.assign(N, 0);
        branchSize.reserve(int(sqrt(N)) * 2);
        branchHead.reserve(int(sqrt(N)) * 2);
        branchTail.reserve(int(sqrt(N)) * 2);
    }


    void addEdge(int u, int v) {
        edges[u].push_back(v);
        edges[v].push_back(u);
    }

    void addEdgeDirected(int u, int v) {
        edges[u].push_back(v);
    }

    void build(int root) {
        this->root = root;
        branchSize.push_back(1);
        branchHead.push_back(root);
        branchTail.push_back(root);
        nodeToBranch[root] = 0;

        dfs(root, -1);
        //dfsIter(root);
        makeLcaTable();

        branchN = int(branchSize.size());
        values.assign(N, defaultValue);
        branchValues.assign(branchN, defaultValue);
        branchLazy.assign(branchN, lzNone);
        branchLazyValues.assign(branchN, defaultValue);
#ifdef USE_LAZY_REBUILDING
        dirty.assign(branchN, false);
#endif
        branchNodes.resize(branchN);
        for (int i = 0; i < branchN; i++)
            branchNodes[i].resize(branchSize[i]);
        for (int u = 0; u < N; u++) {
            int b = nodeToBranch[u];
            branchNodes[b][level[u] - level[branchHead[b]]] = u;
        }
    }

    // call after build()
    void setInitValue(int v, T val) {
        values[v] = val;

        int b = nodeToBranch[v];
        branchValues[b] = mergeOp(branchValues[b], val);
    }

    //--- lca

    //NOTE: findLCA() is faster than lca2()
    int lca2(int u, int v) const {
        while (nodeToBranch[v] != nodeToBranch[u]) {
            if (level[branchHead[nodeToBranch[u]]] > level[branchHead[nodeToBranch[v]]])
                swap(u, v);
            v = branchHead[nodeToBranch[v]];
            v = P[0][v];
        }
        return level[u] <= level[v] ? u : v;
    }

    // kth >= 1, O(sqrt(N))
    // count = valueToCountF(index_of_node, value_or_sqrt_value_of_the_node)
    int climbKth(int v, int kth, const function<int(T)>& valueToCountF) const {
        while (kth > 0) {
            int b = nodeToBranch[v];
            if (v == branchTail[b]) {
                int d = valueToCountF(branchValues[b]);
                if (d >= kth)
                    break;
                kth -= d;
                v = branchHead[b];
                v = P[0][v];
            } else {
                while (kth > 0) {
                    if (valueToCountF(values[v])) {
                        if (--kth == 0)
                            break;
                    }
                    if (v == branchHead[b]) {
                        v = P[0][v];
                        break;
                    }
                    v = P[0][v];
                }
            }
        }

        while (v >= 0 && kth > 0) {
            if (valueToCountF(values[v])) {
                if (--kth == 0)
                    break;
            }
            v = P[0][v];
        }
        return v;
    }

    // from top
    int indexInBranch(int v) const {
        int b = nodeToBranch[v];
        return level[v] - level[branchHead[b]];
    }

    int indexInBranch(int branch, int v) const {
        return level[v] - level[branchHead[branch]];
    }

    //--- LCA

    int climbTree(int node, int dist) const {
        if (dist <= 0)
            return node;

        for (int i = 0; dist > 0; i++) {
            if (dist & 1)
                node = P[i][node];
            dist >>= 1;
        }

        return node;
    }

    int findLCA(int A, int B) const {
        if (level[A] < level[B])
            swap(A, B);

        A = climbTree(A, level[A] - level[B]);

        if (A == B)
            return A;

        int bitCnt = 0;
        for (int x = level[A]; x; x >>= 1)
            bitCnt++;

        for (int i = bitCnt - 1; i >= 0; i--) {
            if (P[i][A] > 0 && P[i][A] != P[i][B]) {
                A = P[i][A];
                B = P[i][B];
            }
        }

        return P[0][A];
    }

    // find LCA when the root is changed
    int findLCA(int root, int A, int B) const {
        int lca = findLCA(A, B);

        int temp = findLCA(A, root);
        if (level[temp] > level[lca])
            lca = temp;

        temp = findLCA(B, root);
        if (level[temp] > level[lca])
            lca = temp;

        return lca;
    }

    int distance(int u, int v) const {
        return level[u] + level[v] - level[findLCA(u, v)] * 2;
    }

    //--- update

    // O(sqrt(N))
    void update(int v, int u, T val) {
        while (nodeToBranch[v] != nodeToBranch[u]) {
            if (level[branchHead[nodeToBranch[u]]] > level[branchHead[nodeToBranch[v]]])
                swap(u, v);

            int b = nodeToBranch[v];
            if (v == branchTail[b])
                updateBranch(b, val);
            else
                updateInBranch(v, branchHead[b], val);
            v = branchHead[b];
            v = P[0][v];
        }
        updateInBranch(v, u, val);
    }

    void add(int v, int u, T val) {
        while (nodeToBranch[v] != nodeToBranch[u]) {
            if (level[branchHead[nodeToBranch[u]]] > level[branchHead[nodeToBranch[v]]])
                swap(u, v);

            int b = nodeToBranch[v];
            if (v == branchTail[b])
                addBranch(b, val);
            else
                addInBranch(v, branchHead[b], val);
            v = branchHead[b];
            v = P[0][v];
        }
        addInBranch(v, u, val);
    }

    //--- query

    // O(sqrt(N))
    T query(int u, int v) {
        T res = defaultValue;
        while (nodeToBranch[v] != nodeToBranch[u]) {
            if (level[branchHead[nodeToBranch[u]]] > level[branchHead[nodeToBranch[v]]])
                swap(u, v);

            int b = nodeToBranch[v];
            if (v == branchTail[b]) {
#ifdef USE_LAZY_REBUILDING
                if (dirty[b]) {
                    recalcBranchValue(b);
                    dirty[b] = false;
                }
#endif
                res = mergeOp(res, branchValues[b]);
            } else {
                res = mergeOp(res, queryInBranch(v, branchHead[b]));
            }
            v = branchHead[b];
            v = P[0][v];
        }
        return mergeOp(res, queryInBranch(v, u));
    }

private:
    //--- DFS

    void dfs(int u, int parent) {
        bool first = true;
        if (branchSize.back() >= sqrtN)
            first = false;

        P[0][u] = parent;
        for (int v : edges[u]) {
            if (v == parent)
                continue;

            if (!first) {
                branchSize.push_back(1);
                branchHead.push_back(v);
                branchTail.push_back(v);
            } else {
                branchTail.back() = v;
                ++branchSize.back();
                first = false;
            }
            nodeToBranch[v] = int(branchSize.size()) - 1;

            level[v] = level[u] + 1;
            dfs(v, u);
        }
    }

    void dfsIter(int root) {
        struct Item {
            int u;
            int parent;
            int vi;         // child index
            bool first;
        };
        vector<Item> st;
        st.reserve(N);

        st.push_back(Item{ root, -1, -1 });
        while (!st.empty()) {
            Item& it = st.back();
            if (++it.vi == 0) {
                // enter ...
                it.first = true;
                if (branchSize.back() >= sqrtN)
                    it.first = false;

                P[0][it.u] = it.parent;
            }
            if (it.vi >= int(edges[it.u].size())) {
                // leave ...
                st.pop_back();
            } else if (edges[it.u][it.vi] != it.parent) {
                // recursion
                int v = edges[it.u][it.vi];

                if (!it.first) {
                    branchSize.push_back(1);
                    branchHead.push_back(v);
                    branchTail.push_back(v);
                } else {
                    branchTail.back() = v;
                    it.first = false;
                }
                nodeToBranch[v] = int(branchSize.size()) - 1;

                level[v] = level[it.u] + 1;
                st.push_back(Item{ v, it.u, -1 });
            }
        }
    }

    void makeLcaTable() {
        for (int i = 1; i < logN; i++) {
            for (int j = 0; j < N; j++) {
                int pp = P[i - 1][j];
                P[i][j] = pp < 0 ? pp : P[i - 1][pp];
            }
        }
    }


    void applyBranch(int branch) {
        if (branchLazy[branch] == lzNone)
            return;

        T val = branchLazyValues[branch];

        int u = branchHead[branch];
        int v = branchTail[branch];
        if (branchLazy[branch] == lzSet) {
            values[v] = val;
            while (v != u) {
                v = P[0][v];
                values[v] = val;
            }
        } else {
            values[v] += val;
            while (v != u) {
                v = P[0][v];
                values[v] += val;
            }
        }

        branchLazy[branch] = lzNone;
        branchLazyValues[branch] = defaultValue;
    }

    void recalcBranchValue(int branch) {
        int u = branchHead[branch];
        int v = branchTail[branch];

        T val = values[v];
        while (v != u) {
            v = P[0][v];
            val = mergeOp(val, values[v]);
        }
        branchValues[branch] = val;
    }

    void updateBranch(int branch, T val) {
        branchValues[branch] = blockOp(val, branchSize[branch]);
        branchLazy[branch] = lzSet;
        branchLazyValues[branch] = val;
    }

    void addBranch(int branch, T val) {
        branchValues[branch] += val * branchSize[branch];   //TODO: fix it
        if (branchLazy[branch] != lzSet)
            branchLazy[branch] = lzAdd;
        branchLazyValues[branch] += val;
    }


    void updateInBranch(int v, int u, T val) {
        if (level[u] > level[v])
            swap(u, v);

        int b = nodeToBranch[v];
        applyBranch(b);

        values[v] = val;
        while (v != u) {
            v = P[0][v];
            values[v] = val;
        }
#ifdef USE_LAZY_REBUILDING
        dirty[b] = true;
#else
        recalcBranchValue(b);
#endif
    }

    void addInBranch(int v, int u, T val) {
        if (level[u] > level[v])
            swap(u, v);

        int b = nodeToBranch[v];
        applyBranch(b);

        values[v] += val;
        while (v != u) {
            v = P[0][v];
            values[v] += val;
        }
#ifdef USE_LAZY_REBUILDING
        dirty[b] = true;
#else
        recalcBranchValue(b);
#endif
    }

    T queryInBranch(int v, int u) {
        if (level[u] > level[v])
            swap(u, v);

        int b = nodeToBranch[v];
        if (branchLazy[b] == lzSet)
            return blockOp(branchLazyValues[b], level[v] - level[u] + 1);

        applyBranch(b);

        T res = values[v];
        while (v != u) {
            v = P[0][v];
            res = mergeOp(res, values[v]);
        }
        
        return res;
    }
};


template <typename T, typename MergeOp, typename BlockOp>
inline TreePathSqrtDecomposition<T, MergeOp, BlockOp> makeTreePathSqrtDecomposition(const MergeOp& mop, const BlockOp& bop, T dfltValue) {
    return TreePathSqrtDecomposition<T, MergeOp, BlockOp>(mop, bop, dfltValue);
}

template <typename T, typename MergeOp, typename BlockOp>
inline TreePathSqrtDecomposition<T, MergeOp, BlockOp> makeTreePathSqrtDecomposition(int size, const MergeOp& mop, const BlockOp& bop, T dfltValue) {
    return TreePathSqrtDecomposition<T, MergeOp, BlockOp>(size, mop, bop, dfltValue);
}
