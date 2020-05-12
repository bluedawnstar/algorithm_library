#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;

#include "suffixArray.h"
#include "suffixArrayAlgo.h"
#include "lcpArraySparseTable.h"

//
// https://www.hackerrank.com/contests/w30/challenges/substring-queries
// 

template <int MaxCharN = 26, int BaseChar = 'a'>
struct LongestCommonStringLengthWithSuffixArray {
    int mN;                                             // the number of strings
    int mQ;                                             // the number of queries
    vector<pair<int, int>> mS;
    string mSS;

    vector<int> mSuffixToStrID;

    SuffixArray<MaxCharN + 1,BaseChar> mSA;

    vector<int> mAns;
    vector<pair<int, int>> mQIn;
    unordered_map<int, unordered_map<int, int>> mQIndex;// (L, R) -> Q index
    vector<unordered_map<int, int>> mQuery;             // R --> (L, Q index)

    // 
    // 1) how to reduce redundant questions
    //      mQuery[R][L] = the index of answer to (L, R) or (R, L)
    // 2) how to reduce the number of comparing between two suffixes 
    //      update forward and backward instead of making all pairs
    // 3) how to use suffix array, LCP array, sparse table, etc
    //      
    vector<int> solve(const vector<string>& strs, const vector<pair<int,int>>& query) {
        mN = int(strs.size());
        mQ = int(query.size());

        int totLen = 0;
        for (const string& s : strs)
            totLen += int(s.length());
        mSuffixToStrID.resize(mN + totLen + 1);

        mS.clear();
        mSS.clear();
        mS.reserve(mN);
        for (int i = 0; i < mN; i++) {
            mS.emplace_back(int(mSS.length()), int(mSS.length() + strs[i].length()));

            int len = int(mSS.length());
            mSS += strs[i];
            mSS += char('z' + 1);

            for (int j = 0; j < int(strs[i].length()); j++)
                mSuffixToStrID[len + j] = i;
            mSuffixToStrID[len + strs[i].length()] = -1;
        }

        mAns.resize(mQ);
        mQIn.resize(mQ);
        mQuery.resize(mN);   // R --> (L, Q index)

        // grouping queries for speed up
        for (int i = 0; i < mQ; i++) {
            int L = query[i].first;
            int R = query[i].second;

            if (L > R)
                swap(L, R);

            mQIn[i] = make_pair(L, R);
            if (mQIndex[L].find(R) == mQIndex[L].end())
                mQIndex[L][R] = i;

            if (L == R)
                mAns[mQIndex[L][R]] = int(mS[L].second) - int(mS[L].first);
            else
                mQuery[R][L] = mQIndex[L][R];
        }

        // making suffix array, LCP array, sparse table for LCP array
        mSA.build(mSS);

        // check forward
        vector<int> lastSAIndex(mN, -1);
        int prevStrID = -1;
        for (int i = 0; i < int(mSA.size()); i++) {
            int rStrID = mSuffixToStrID[mSA[i]];
            if (rStrID < 0)
                continue;

            if (prevStrID != rStrID) {
                for (auto it : mQuery[rStrID]) {
                    int lStrID = it.first;
                    if (lastSAIndex[lStrID] < 0)
                        continue;

                    int lcp = mSA.lcp(lastSAIndex[lStrID], i);

                    int maxLen = min(mS[lStrID].second - mSA[lastSAIndex[lStrID]],
                                     mS[rStrID].second - mSA[i]);
                    if (lcp > maxLen)
                        lcp = maxLen;

                    mAns[it.second] = max(mAns[it.second], lcp);
                }
            }
            lastSAIndex[rStrID] = i;
            prevStrID = rStrID;
        }

        // check backward
        fill(lastSAIndex.begin(), lastSAIndex.end(), -1);
        prevStrID = -1;
        for (int i = int(mSA.size()) - 1; i >= 0; i--) {
            int rStrID = mSuffixToStrID[mSA[i]];
            if (rStrID < 0)
                continue;

            if (prevStrID != rStrID) {
                for (auto it : mQuery[rStrID]) {
                    int lStrID = it.first;
                    if (lastSAIndex[lStrID] < 0)
                        continue;

                    int lcp = mSA.lcp(i, lastSAIndex[lStrID]);

                    int maxLen = min(mS[lStrID].second - mSA[lastSAIndex[lStrID]],
                                     mS[rStrID].second - mSA[i]);
                    if (lcp > maxLen)
                        lcp = maxLen;

                    mAns[it.second] = max(mAns[it.second], lcp);
                }
            }
            lastSAIndex[rStrID] = i;
            prevStrID = rStrID;
        }

        // making answer
        vector<int> res(mQ);
        for (int i = 0; i < mQ; i++)
            res[i] = mAns[mQIndex[mQIn[i].first][mQIn[i].second]];

        return res;
    }
};

#include "suffixAutomaton.h"

template <int MaxCharN = 26, int BaseChar = 'a'>
struct LongestCommonStringLengthWithSuffixAutomation {
    int mN;                                             // the number of strings
    int mQ;                                             // the number of queries

    vector<int> mAns;
    vector<pair<int, int>> mQIn;
    unordered_map<int, unordered_map<int, int>> mQIndex;// (L, R) -> Q index
    vector<unordered_map<int, int>> mQuery;             // R --> (L, Q index)

    vector<SuffixAutomaton<MaxCharN,BaseChar>> mSA;

    vector<int> solve(const vector<string>& strs, const vector<pair<int, int>>& query) {
        mN = int(strs.size());
        mQ = int(query.size());

        for (int i = 0; i < mN; i++) {
            SuffixAutomaton<MaxCharN,BaseChar> t(int(strs[i].length()));
            t.extend(strs[i]);
            mSA.emplace_back(move(t));
        }

        mAns.resize(mQ);
        mQIn.resize(mQ);
        mQuery.resize(mN);   // R --> (L, Q index)

        // grouping queries for speed up
        for (int i = 0; i < mQ; i++) {
            int L = query[i].first;
            int R = query[i].second;

            if (L > R)
                swap(L, R);

            mQIn[i] = make_pair(L, R);
            if (mQIndex[L].find(R) == mQIndex[L].end())
                mQIndex[L][R] = i;

            if (L == R)
                mAns[mQIndex[L][R]] = int(strs[L].length());
            else
                mQuery[R][L] = mQIndex[L][R];
        }

        for (int i = mN - 1; i > 0; i--) {
            if (mQuery[i].empty())
                continue;

            for (auto it : mQuery[i]) {
                if (strs[i].length() > strs[it.first].length())
                    mAns[it.second] = lcs(mSA[i], strs[it.first]);
                else
                    mAns[it.second] = lcs(mSA[it.first], strs[i]);
            }
        }

        // making answer
        vector<int> res(mQ);
        for (int i = 0; i < mQ; i++)
            res[i] = mAns[mQIndex[mQIn[i].first][mQIn[i].second]];

        return res;
    }

private:
    int lcs(SuffixAutomaton<MaxCharN,BaseChar>& sa, const string& t) {
        int v = 0, l = 0, best = 0, bestpos = 0;
        for (int i = 0; i < int(t.length()); ++i) {
            int ch = SuffixAutomaton<MaxCharN,BaseChar>::ch2i(t[i]);
            while (v && !sa.state[v].next[ch]) {
                v = sa.state[v].suffixLink;
                l = sa.state[v].len;
            }
            if (sa.state[v].next[ch]) {
                v = sa.state[v].next[ch];
                ++l;
            }
            if (l > best)
                best = l, bestpos = i;
        }
        return best;
    }
};


/////////// For Testing ///////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include "../common/iostreamhelper.h"


void testLongestCommonSubstring_queryTwoStringAmongStrings() {
    return; //TODO: if you want to test string functions, make this line a comment.

    cout << "-- Testing for Longest Common Substring Length Queries -------------" << endl;

    vector<string> s1 = {
        "vyemcyjagcttykawuoixitmucywoqkyweqmfonmrsimgifciaesivodsleovqeyaownngwizswmgaaluvryeaosznwuugonwfmasewaqesqcacqievglnyk",
        "mkascndgjaqmzqgghkgkqcikixyaaimcguqukgyzywmxgjcfqnwsawfgusscwkucvmyijvqsiscbiqcanmayerpausgiaabeskmyocjaewrplmqawmufwyhwcvcagviswqmuomanitkkzargkocsaguacasiqoboavfifhoaqgbmglzueksomxouucybzmazggakvokvvaauggjnobcaxfxaooeqswiwwsnmylamwkjwluqyaueyakooeiilzahvmcaqabwxyqisayoljhwogyqamuajgquacxogkihsaaveibymqaamtuheofwcarfuasdgewqartwakaimtwfkkanwenumsywuwswarqkbqoygewkamnyosghiueoyyoueyniizagondaseqenasmecfwgikraalgecvtmmvcuagimnoqemnrtkskcshunhkibgqjluagwysgtsgqwopypwmdiewawtmyisauostxjquwaaafqyswixtyeaakezjnoluanpkpckcgisaekiedaaeqarmcgsapdkiaecoskysancgjciahyaevscoosoawilxykzuyeadeupcuyuuqaaqpcmyfatawusyezzvnalebveomotkcsmsfwaigyojmintquqmwbyexyuviopgyeacguoasfuyaezwemekmmvugkikucwcwwgyiesavoaugwswaxalqczqaabnmovgimqutwhaaguaauytipkyeeywxgienornltchyaaycauu",
        "kgaylee",
        "a",
        "ausew",
        "mqugrs",
        "a",
        "m",
        "w",
        "a",
        "odg",
        "k",
        "y",
        "v",
        "a",
        "g",
        "g",
        "w",
        "m",
        "d",
        "a",
        "z",
        "k",
        "v",
        "s",
        "q",
        "f",
        "v",
        "w",
        "g",
        "f",
        "n",
        "a",
        "i",
        "g",
        "m",
        "o",
        "j",
        "g",
        "e",
        "a",
        "i",
        "a",
        "q",
        "q",
        "i",
        "y",
        "n",
        "v",
        "o",
        "a",
        "n",
        "a",
        "d",
        "w",
        "i",
        "u",
        "o",
        "i",
        "j",
        "l",
        "w",
        "k",
        "d",
        "j",
        "q",
        "f",
        "c",
        "m",
        "c",
        "o",
        "y",
        "g",
        "i",
        "u",
        "u",
        "q",
        "s",
        "o",
        "i",
        "t",
        "m",
        "c",
        "j",
        "y",
        "a",
        "k",
        "i",
        "s",
        "p",
        "b",
        "p",
        "y",
        "e",
        "c",
        "u",
        "k",
        "q",
        "t",
        "q"
    };
    vector<pair<int, int>> q1 = {
        { 49, 12 },
        { 29, 44 },
        { 32,  3 },
        { 39, 13 },
        { 76, 71 },
        { 43, 47 },
        { 46, 67 },
        { 50, 53 },
        { 95, 10 },
        { 11, 78 },
        { 14, 23 },
        { 82, 37 },
        { 39, 25 },
        { 21, 78 },
        { 81, 55 },
        { 42, 71 },
        { 47, 34 },
        { 19, 23 },
        { 59, 58 },
        { 95, 35 },
        { 96, 33 },
        { 17, 70 },
        { 64, 28 },
        {  9, 70 },
        {  1, 78 },
        { 54, 19 },
        { 61, 96 },
        { 22, 32 },
        { 71, 26 },
        { 17, 20 },
        { 84, 90 },
        { 44, 67 },
        { 70, 38 },
        {  4, 21 },
        { 20, 82 },
        { 46, 86 },
        { 62, 92 },
        { 36, 62 },
        {  2, 43 },
        { 23,  2 },
        { 71, 74 },
        { 39, 64 },
        { 33, 25 },
        { 42, 96 },
        {  9, 72 },
        { 21,  4 },
        {  3, 95 },
        { 31, 46 },
        { 97, 70 },
        { 39, 94 },
        { 61, 18 },
        { 49, 81 },
        { 54, 95 },
        { 76, 84 },
        {  3,  8 },
        { 66, 77 },
        { 73, 40 },
        { 91, 33 },
        { 26, 10 },
        { 26, 97 },
        {  5, 31 },
        { 26, 70 },
        {  8, 74 },
        { 56, 71 },
        { 56,  8 },
        { 50, 28 },
        { 64, 58 },
        { 84,  9 },
        { 64, 60 },
        { 16, 19 },
        {  7, 31 },
        { 35, 67 },
        { 59, 21 },
        { 70, 69 },
        { 33, 35 },
        { 14, 78 },
        { 92, 31 },
        {  0, 81 },
        {  5, 75 },
        { 59, 86 },
        { 19, 62 },
        { 99, 60 },
        { 32, 78 },
        { 50, 66 },
        { 97, 66 },
        { 41, 44 },
        { 47, 72 },
        { 14, 79 },
        { 22, 93 },
        {  7, 40 },
        { 72, 68 },
        { 98, 88 },
        { 77,  4 },
        { 10, 90 },
        { 22, 34 },
        {  0, 68 },
        { 35,  7 },
        { 83, 57 },
        { 66, 70 },
        { 97, 55 },
    };
    vector<int> ans1 = {
        { 0 },
        { 0 },
        { 1 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 1 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 1 },
        { 1 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 0 },
        { 1 },
        { 0 },
        { 0 },
        { 1 },
        { 1 },
        { 0 },
        { 0 },
        { 0 },
    };

    LongestCommonStringLengthWithSuffixArray<> lcsSuffixArray;
    assert(lcsSuffixArray.solve(s1, q1) == ans1);

    LongestCommonStringLengthWithSuffixAutomation<> lcsSuffixAutomation;
    assert(lcsSuffixAutomation.solve(s1, q1) == ans1);

    cout << "OK!" << endl;
}
