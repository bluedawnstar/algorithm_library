#include <iostream>

using namespace std;

#define TEST(ModuleName)    extern void test##ModuleName(void); \
                            test##ModuleName()

int main(void) {
    TEST(Subsequence);
    TEST(SubsequenceSpecial);
    TEST(LongestCommonSubsequence);
    TEST(PalindromicSubsequence);
    TEST(PalindromicSubstring);
    TEST(Knapsack);
    TEST(BitDP);
    TEST(DigitDP);
    TEST(ProductSumOfSubarray);
    TEST(PersistentConvexHull);
    TEST(DynamicUpperEnvelope);
    TEST(DynamicLowerEnvelope);
    TEST(DynamicConvexHull);
    TEST(ConvexHullTrick);
    TEST(DivideAndConquerOptimization);
    TEST(OptimalSquareDistance1D);
    TEST(KnuthOptimization);
    TEST(BitonicTour);
    TEST(EggDropping);
    TEST(BrokenProfile);
    TEST(BrokenProfileEx);
    TEST(PartitionProblem);
    TEST(PartitionProblemMultiple);
    TEST(BitLCS);
    TEST(CyclicLCS);
    TEST(LongestIncreasingSubsequence);
    TEST(LongestRepeatedSubsequence);
    TEST(MaxSubarraySum);
    TEST(SelectBlock2xN);
    TEST(IntegerPartition);
    TEST(TwoSameSumSubset);
    TEST(PowerSumOfAllCombinations);
    TEST(MaxWeightedNonoverlappingIntervals);
}
