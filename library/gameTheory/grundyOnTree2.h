#pragma once

// https://www.hackerearth.com/practice/algorithms/graphs/depth-first-search/practice-problems/algorithm/tree-game-2-e6796bef/description/

template <int MaxBitN = 31>
struct GrundyOnTree2 {
    int N;
    vector<vector<int>> edges;

    PersistentMexTrie<MaxBitN> mexTrie;
    vector<int> roots;

    vector<int> lazy;
    vector<int> grundy; // grundy[u] = grundy number of sub-tree of u

    GrundyOnTree2() : N(0) {
    }

    explicit GrundyOnTree2(int n) {
        init(n);
    }

    void init(int n) {
        N = n;
        edges = vector<vector<int>>(n);

        mexTrie.init();
        roots = vector<int>(n);

        lazy = vector<int>(n);
        grundy = vector<int>(n);
    }

    void addEdge(int u, int v) {
        edges[u].push_back(v);
        edges[v].push_back(u);
    }

    //---

    void build(int root) {
        dfsGrundy(root, -1);
    }

    int getGrundy(int u) const {
        return grundy[u];
    }

private:
    /*
        grundy number G

        G[u] = mex{ G[u][subgame-0],
                    G[u][subgame-1],
                    G[u][subgame-2],
                    ...
                  }
             = mex{ G[subgame-0][v0] ^ G[subgame-0][v1] ^ ... ^ G[subgame-0][v_i] ^ ...,
                    G[subgame-1][v0] ^ G[subgame-1][v1] ^ ... ^ G[subgame-1][v_j] ^ ...,
                    G[subgame-2][v0] ^ G[subgame-2][v1] ^ ... ^ G[subgame-2][v_k] ^ ...,
                    ...
                  }
    */
    void dfsGrundy(int u, int parent) {
        int childGrundy = 0;
        for (auto v : edges[u]) {
            if (v == parent)
                continue;
            dfsGrundy(v, u);
            childGrundy ^= grundy[v];
        }

        for (auto v : edges[u]) {
            if (v == parent)
                continue;
            lazy[v] ^= childGrundy ^ grundy[v];
            // small-to-large
            if (mexTrie.count(roots[v]) > mexTrie.count(roots[u])) {
                swap(roots[u], roots[v]);
                swap(lazy[u], lazy[v]);
            }
            // merge
            //roots[u] = mexTrie.merge(roots[u], roots[v], lazy[u] ^ lazy[v]);
            roots[u] = mexTrie.mergeMove(roots[u], roots[v], lazy[u] ^ lazy[v]);
        }
        grundy[u] = mexTrie.mex(roots[u], lazy[u]);
        roots[u] = mexTrie.insert(roots[u], childGrundy ^ lazy[u]).first;
    }
};
