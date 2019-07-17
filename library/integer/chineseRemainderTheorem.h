#pragma once

#include "intMod.h"

// https://librewiki.net/wiki/%EC%A4%91%EA%B5%AD%EC%9D%B8%EC%9D%98_%EB%82%98%EB%A8%B8%EC%A7%80_%EC%A0%95%EB%A6%AC 
// https://e-maxx-eng.appspot.com/algebra/chinese-remainder-theorem.html
// https://forthright48.com/chinese-remainder-theorem-part-1-coprime-moduli/
// https://forthright48.com/chinese-remainder-theorem-part-2-non-coprime-moduli/

/*
�߱����� ������ ������, � �� x�� ���μ��� m1, m2, m3, ..., mn���� ���� ������ �� �������� r1, r2, r3, ..., rn �̶�� �� ��
x�� m1* m2 * m3 * ... * mn �� modular ���� �ȿ��� �ذ� �����ϴٴ� ���� ���Ѵ�.
�� x�� ���ϴ� ������ ������ ����.

1) m = m1 * m2 * m3 * ... * mn�̶�� �Ҷ�, nk = m / mk��� ����.
   nk * sk = 1 (mod mk) �� sk�� ���Ѵ�(1 <= k <= n)
   (Modular Multiplicative Inverse)
2) x = r1 * n1 * s1 + r2 * n2 * s2 + ... + rk * nk * sk + ... + rn * nn * sn (mod m)
*/

// find minimum X (vector version)
// X % m[i] = r[i], all m[i] are coprime
template <typename T>
pair<T, T> solveCRT(const vector<T>& m, const vector<T>& r, int n) {
    T mm = 1;
    for (int i = 0; i < n; i++)
        mm *= m[i];

    T res = 0;

    for (int i = 0; i < n; i++) {
        T nk = mm / m[i];
        res += r[i] * nk * modInv(nk, m[i]);
    }

    return make_pair(res % mm, mm);
}

// find minimum X (array version)
// X % m[i] = r[i], all m[i] are coprime
template <typename T>
pair<T, T> solveCRT(const T m[], const T r[], int n) {
    T mm = 1;
    for (int i = 0; i < n; i++)
        mm *= m[i];

    T res = 0;

    for (int i = 0; i < n; i++) {
        T nk = mm / m[i];
        res += r[i] * nk * modInv(nk, m[i]);
    }

    return make_pair(res % mm, mm);
}


// Chinese remainder theorem (special case):
//      find x satisfying x % m1 = r1, x % m2 = r2 (x is unique modulo M = lcm(m1, m2))
// return (x, M), if failed then (0, -1)
template <typename T>
inline pair<T, T> solveCRTExt(T m1, T r1, T m2, T r2) {
    T s, t;
    T g = extGcd(m1, m2, s, t);
    if (r1 % g != r2 % g)
        return make_pair(0, -1);
    T m = m1 * m2;
    return make_pair((((s * r2 * m1 + t * r1 * m2) % m + m) % m) / g, m / g);
}

// Chinese remainder theorem:
//      find x satisfying x % m[i] = r[i] for all i (x is unique modulo M = lcm(m[i]))
// return (x, M), if failed then (0, -1)
// 
// NOTE: it is not necessary that all m[i] are coprime
template <typename T>
inline pair<T, T> solveCRTExt(const vector<T>& m, const vector<T>& r, int n) {
    auto res = make_pair(r[0], m[0]);
    for (int i = 1; i < n; i++) {
        res = solveCRTExt(res.second, res.first, m[i], r[i]);
        if (res.second == -1)
            break;
    }
    return res;
}

// Chinese remainder theorem:
//      find x satisfying x % m[i] = r[i] for all i (x is unique modulo M = lcm(m[i]))
// return (x, M), if failed then (0, -1)
// 
// NOTE: it is not necessary that all m[i] are coprime
template <typename T>
inline pair<T, T> solveCRTExt(const T m[], const T r[], int n) {
    auto res = make_pair(r[0], m[0]);
    for (int i = 1; i < n; i++) {
        res = solveCRTExt(res.second, res.first, m[i], r[i]);
        if (res.second == -1)
            break;
    }
    return res;
}
