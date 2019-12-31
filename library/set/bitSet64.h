#pragma once

#ifndef __GNUC__
#include <intrin.h>
#endif
#include <immintrin.h>

// BitSet64 class with mBitCnt member variable
struct BitSet64 {
    static int clz(unsigned long long x) {
#if defined(_M_X64)
        return int(_lzcnt_u64(x));
#elif defined(__GNUC__)
        return __builtin_clzll(x);
#else
        if ((x >> 32) != 0)
            return int(_lzcnt_u32(unsigned(x >> 32)));
        else
            return 32 + int(_lzcnt_u32(unsigned(x)));
#endif
    }

    static int popCount(unsigned long long x) {
#if defined(_M_X64)
        return int(__popcnt64(x));
#elif defined(__GNUC__)
        return __builtin_popcountll(x);
#else
        return int(__popcnt(unsigned(x))) + int(__popcnt(unsigned(x >> 32)));
#endif
    }

    static const int BIT_SIZE = sizeof(unsigned long long) * 8;
    static const unsigned long long BIT_ALL = (unsigned long long)-1ll;
    static const unsigned long long BIT_ONE = 1ull;

    static const int INDEX_MASK = 0x3F;
    static const int INDEX_SHIFT = 6;

    int mN;
    int mBitCnt;

    unsigned long long mEndMask;
    vector<unsigned long long> mV;

    BitSet64() {
    }

    explicit BitSet64(int n) {
        init(n);
    }

    void init(int N) {
        mN = N;
        mBitCnt = 0;

        int r = N % BIT_SIZE;
        mEndMask = r ? ((BIT_ONE << r) - BIT_ONE) : BIT_ALL;
        mV = vector<unsigned long long>((N + BIT_SIZE - 1) / BIT_SIZE);
    }

    int size() const {
        return mN;
    }

    int count() const {
        return mBitCnt;
    }

    bool all() const {
        return mBitCnt >= mN;
    }

    bool any() const {
        return mBitCnt > 0;
    }

    bool none() const {
        return mBitCnt <= 0;
    }

    bool test(int pos) const {
        return (mV[pos >> INDEX_SHIFT] & (BIT_ONE << (pos & INDEX_MASK))) != 0;
    }

    BitSet64& set() {
        if (mBitCnt < mN) {
            int n = mN >> INDEX_SHIFT;
            for (int i = 0; i < n; i++)
                mV[i] = BIT_ALL;
            if (mEndMask + 1 != 0)
                mV[n] = mEndMask;

            mBitCnt = mN;
        }
        return *this;
    }

    BitSet64& set(int pos, bool value = true) {
        int idx = pos >> INDEX_SHIFT;
        int off = pos & INDEX_MASK;
        if (value) {
            if ((mV[idx] & (BIT_ONE << off)) == 0) {
                mV[idx] |= BIT_ONE << off;
                mBitCnt++;
            }
        } else {
            if (mV[idx] & (BIT_ONE << off)) {
                mV[idx] &= ~(BIT_ONE << off);
                mBitCnt--;
            }
        }
        return *this;
    }

    BitSet64& reset() {
        if (mBitCnt > 0) {
            int n = (mN + BIT_SIZE - 1) >> INDEX_SHIFT;
            for (int i = 0; i < n; i++)
                mV[i] = 0;
            mBitCnt = 0;
        }
        return *this;
    }

    BitSet64& reset(int pos) {
        int idx = pos >> INDEX_SHIFT;
        int off = pos & INDEX_MASK;
        if (mV[idx] & (BIT_ONE << off)) {
            mV[idx] &= ~(BIT_ONE << off);
            mBitCnt--;
        }

        return *this;
    }

    BitSet64& flip() {
        int n = mN >> INDEX_SHIFT;
        for (int i = 0; i < n; i++)
            mV[i] ^= BIT_ALL;
        if (mEndMask + 1 != 0)
            mV[n] ^= mEndMask;

        mBitCnt = mN - mBitCnt;

        return *this;
    }

    BitSet64& flip(int pos) {
        int idx = pos >> INDEX_SHIFT;
        int off = pos & INDEX_MASK;
        if ((mV[idx] & (BIT_ONE << off)) == 0) {
            mV[idx] |= BIT_ONE << off;
            mBitCnt++;
        } else {
            mV[idx] &= ~(BIT_ONE << off);
            mBitCnt--;
        }
        return *this;
    }

    bool operator ==(const BitSet64& rhs) const {
        if (mBitCnt != rhs.mBitCnt)
            return false;

        int n1 = (mN + BIT_SIZE - 1) >> INDEX_SHIFT;
        int n2 = (rhs.mN + BIT_SIZE - 1) >> INDEX_SHIFT;
        if (n1 != n2)
            return false;

        for (int i = 0; i < n1; i++) {
            if (mV[i] != rhs.mV[i])
                return false;
        }
        return true;
    }

    bool operator !=(const BitSet64& rhs) const {
        return !operator ==(rhs);
    }

    BitSet64& operator |=(const BitSet64& rhs) {
        int n1 = (mN + BIT_SIZE - 1) >> INDEX_SHIFT;
        int n2 = (rhs.mN + BIT_SIZE - 1) >> INDEX_SHIFT;

        int n = min(n1, n2);
        for (int i = 0; i < n; i++)
            mV[i] |= rhs.mV[i];
        recalcCount();
        return *this;
    }

    BitSet64& operator &=(const BitSet64& rhs) {
        int n1 = (mN + BIT_SIZE - 1) >> INDEX_SHIFT;
        int n2 = (rhs.mN + BIT_SIZE - 1) >> INDEX_SHIFT;

        int n = min(n1, n2);
        for (int i = 0; i < n; i++)
            mV[i] &= rhs.mV[i];
        recalcCount();
        return *this;
    }

    BitSet64& operator ^=(const BitSet64& rhs) {
        int n1 = (mN + BIT_SIZE - 1) >> INDEX_SHIFT;
        int n2 = (rhs.mN + BIT_SIZE - 1) >> INDEX_SHIFT;

        int n = min(n1, n2);
        for (int i = 0; i < n; i++)
            mV[i] ^= rhs.mV[i];
        recalcCount();
        return *this;
    }

    BitSet64 operator |(const BitSet64& rhs) {
        BitSet64 bs = *this;
        bs.init(mN);

        int n = mN >> INDEX_SHIFT;
        for (int i = 0; i < n; i++)
            bs.mV[i] = mV[i] | rhs.mV[i];
        if (mEndMask + 1 != 0)
            bs.mV[n] = (mV[n] | rhs.mV[n]);
        bs.recalcCount();
        return bs;
    }

    BitSet64 operator &(const BitSet64& rhs) {
        BitSet64 bs = *this;
        bs.init(mN);

        int n = mN >> INDEX_SHIFT;
        for (int i = 0; i < n; i++)
            bs.mV[i] = mV[i] & rhs.mV[i];
        if (mEndMask + 1 != 0)
            bs.mV[n] = mV[n] & rhs.mV[n];
        bs.recalcCount();
        return bs;
    }

    BitSet64 operator ^(const BitSet64& rhs) {
        BitSet64 bs = *this;
        bs.init(mN);

        int n = mN >> INDEX_SHIFT;
        for (int i = 0; i < n; i++)
            bs.mV[i] = mV[i] ^ rhs.mV[i];
        if (mEndMask + 1 != 0)
            bs.mV[n] = mV[n] ^ rhs.mV[n];
        bs.recalcCount();
        return bs;
    }

    BitSet64 operator ~() const {
        BitSet64 bs = *this;
        bs.init(mN);

        int n = mN >> INDEX_SHIFT;
        for (int i = 0; i < n; i++)
            bs.mV[i] = ~mV[i];
        if (mEndMask + 1 != 0)
            bs.mV[n] = ~mV[n] & mEndMask;

        bs.mBitCnt = mN - mBitCnt;
        return bs;
    }

    BitSet64& operator <<=(int n) {
        if (n <= 0)
            return *this;

        if (n >= mN) {
            fill(mV.begin(), mV.end(), 0);
            mBitCnt = 0;
            return *this;
        }

        int d = n >> INDEX_SHIFT;
        int r = n & INDEX_MASK;

        if (r == 0) {
            int t = (mN - 1) >> INDEX_SHIFT;
            int s = t - d;

            while (s >= 0)
                mV[t--] = mV[s--];
            while (t >= 0)
                mV[t--] = 0;
        } else {
            int t = (mN - 1) >> INDEX_SHIFT;
            int s1 = t - d;
            int s0 = s1 - 1;

            while (s0 >= 0)
                mV[t--] = (mV[s1--] << r) | (mV[s0--] >> (BIT_SIZE - r));
            if (s1 >= 0)
                mV[t--] = (mV[s1--] << r);
            while (t >= 0)
                mV[t--] = 0;
        }
        if (mEndMask + 1 != 0)
            mV[mN >> INDEX_SHIFT] &= mEndMask;

        recalcCount();
        return *this;
    }

    BitSet64& operator >>=(int n) {
        if (n <= 0)
            return *this;

        if (n >= mN) {
            fill(mV.begin(), mV.end(), 0);
            mBitCnt = 0;
            return *this;
        }

        int d = n >> INDEX_SHIFT;
        int r = n & INDEX_MASK;

        int VN = int(mV.size());

        if (r == 0) {
            int t = 0;
            int s = t + d;

            while (s < VN)
                mV[t++] = mV[s++];
            while (t < VN)
                mV[t++] = 0;
        } else {
            int t = 0;
            int s0 = t + d;
            int s1 = s0 + 1;

            while (s1 < VN)
                mV[t++] = (mV[s0++] >> r) | (mV[s1++] << (BIT_SIZE - r));
            if (s0 < VN)
                mV[t++] = (mV[s0++] >> r);
            while (t < VN)
                mV[t++] = 0;
        }
        if (mEndMask + 1 != 0)
            mV[mN >> INDEX_SHIFT] &= mEndMask;

        recalcCount();
        return *this;
    }

    //-----------------------------------------------------

    int recalcCount() {
        mBitCnt = 0;
        for (int i = 0; i < int(mV.size()); i++)
            mBitCnt += popCount(mV[i]);
        return mBitCnt;
    }

    int firstClearBit() const {
        for (int i = 0; i < int(mV.size()); i++) {
            if (mV[i] != BIT_ALL) {
                long long m = (long long)~mV[i];
                return (i << INDEX_SHIFT) + BIT_SIZE - clz((unsigned long long)(m & -m)) - 1;
            }
        }
        return size();
    }

    int first() const {
        for (int i = 0; i < int(mV.size()); i++) {
            if (mV[i]) {
                long long m = (long long)mV[i];
                return (i << INDEX_SHIFT) + BIT_SIZE - clz((unsigned long long)(m & -m)) - 1;
            }
        }
        return -1;
    }

    int last() const {
        for (int i = int(mV.size()) - 1; i >= 0; i--) {
            if (mV[i])
                return (i << INDEX_SHIFT) + BIT_SIZE - clz(mV[i]) - 1;
        }
        return -1;
    }

    // pos < next(pos) < N (or -1)
    int next(int pos) const {
        if (++pos >= mN)
            return -1;

        int index = pos >> INDEX_SHIFT;
        int offset = pos & INDEX_MASK;

        long long m = (long long)mV[index] & (BIT_ALL << offset);
        if (m)
            return (index << INDEX_SHIFT) + BIT_SIZE - clz((unsigned long long)(m & -m)) - 1;

        for (int i = index + 1; i < int(mV.size()); i++) {
            if (mV[i]) {
                m = (long long)mV[i];
                return (i << INDEX_SHIFT) + BIT_SIZE - clz((unsigned long long)(m & -m)) - 1;
            }
        }

        return -1;
    }

    // -1 <= prev(pos) < pos
    int prev(int pos) const {
        if (--pos < 0)
            return -1;

        int index = pos >> INDEX_SHIFT;
        int offset = pos & INDEX_MASK;

        long long m = (long long)mV[index] & (BIT_ALL >> (BIT_SIZE - 1 - offset));
        if (m)
            return (index << INDEX_SHIFT) + BIT_SIZE - clz(m) - 1;

        for (int i = index - 1; i >= 0; i--) {
            if (mV[i])
                return (i << INDEX_SHIFT) + BIT_SIZE - clz(mV[i]) - 1;
        }

        return -1;
    }
};
