#pragma once

template <typename T>
struct PrefixSum2D {
    vector<vector<T>> sum;

    PrefixSum2D() {
    }

    explicit PrefixSum2D(const vector<vector<T>>& in) {
        build(in);
    }

    void build(const vector<vector<T>>& in) {
        int H = int(in.size());
        int W = int(in[0].size());

        sum = vector<vector<T>>(H + 1, vector<T>(W + 1));
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                sum[i + 1][j + 1] = sum[i][j + 1] + sum[i + 1][j] - sum[i][j] + in[i][j];
            }
        }
    }

    // inclusive (0 <= left <= right < N), (0 <= top <= bottom < N)
    T query(int left, int top, int right, int bottom) const {
        return sum[bottom + 1][right + 1] - sum[bottom + 1][left] - sum[top][right + 1] + sum[top][left];
    }
};
