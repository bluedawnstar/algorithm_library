#pragma once

#include "intMod.h"

// Returns value of n! % p
inline int factorialModNaive(int n, int M) {
    if (n >= M)
        return 0;

    int result = 1;
    for (int i = 1; i <= n; i++)
        result = int((long long)result * i % M);

    return result;
}

// https://acmcairoscience.wordpress.com/2015/04/06/factorial-calculation-modulo/
// O(M*log_M(N))
inline int factorialMod(int n, int M) {
    long long res = 1;
    while (n > 1) {
        res = (res * ((n / M) % 2 ? M - 1 : 1)) % M;
        for (int i = 2; i <= n % M; ++i)
            res = (res * i) % M;
        n /= M;
    }
    return int(res % M);
}


/*
<Wilson's Theorem>
        (p - 1)! �� -1 (mod p)    (p > 1, p is a prime number)
    OR  (p - 1)! �� (p - 1) (mod p)
*/

// returns n! % M using Wilson's Theorem
// It is efficient when n is close to M

// PRECONDITION: M is a prime number
inline int factorialModWilson(int n, int M) {
    // n! % M is 0 if n >= M
    if (n >= M)
        return 0;

    // Initialize result as (M - 1)! which is -1 or (M - 1)
    int res = (M - 1);
    for (int i = n + 1; i < M; i++)
        res = int((long long)res * modInvPrime(i, M) % M);

    return res;
}

inline long long factorialModWilson(long long n, long long M) {
    // n! % M is 0 if n >= M
    if (n >= M)
        return 0;

    // Initialize result as (M - 1)! which is -1 or (M - 1)
    long long res = (M - 1);
    for (long long i = n + 1; i < M; i++)
        res = res * modInvPrime(i, M) % M;

    return res;
}
