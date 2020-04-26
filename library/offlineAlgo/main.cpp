#include <iostream>

using namespace std;

#define TEST(ModuleName)    extern void test##ModuleName(void); \
                            test##ModuleName()

int main(void) {
    TEST(CountUniqueNumbers);
    TEST(SegmentMinValueFinder);
    TEST(LcaTarjan);
    TEST(OfflineRMQ);
    TEST(MOAlgorithm);
    TEST(XTimesValueChecker);
}
