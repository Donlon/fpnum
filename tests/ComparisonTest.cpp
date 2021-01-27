#include <fpnum.h>
#include <fp8.h>
#include <gtest/gtest.h>

template<unsigned int e, unsigned int m>
void testGt(const fpnum<e, m> &num1, const fpnum<e, m> &num2) {
    EXPECT_TRUE(num1 > num2);
    EXPECT_TRUE(num2 < num1);
    EXPECT_FALSE(num1 <= num2);
    EXPECT_FALSE(num2 >= num1);
}

template<unsigned int e, unsigned int m>
void testGtEq(const fpnum<e, m> &num1, const fpnum<e, m> &num2) {
    EXPECT_TRUE(num1 >= num2);
    EXPECT_TRUE(num2 <= num1);
    EXPECT_FALSE(num1 < num2);
    EXPECT_FALSE(num2 > num1);
}

template<unsigned int e, unsigned int m>
void testEq(const fpnum<e, m> &num1, const fpnum<e, m> &num2) {
    EXPECT_TRUE(num1 == num2);
    EXPECT_FALSE(num1 != num2);
    EXPECT_TRUE(num2 == num1);
    EXPECT_FALSE(num2 != num1);
    EXPECT_TRUE(num1 <= num2);
    EXPECT_TRUE(num2 <= num1);
    EXPECT_TRUE(num1 >= num2);
    EXPECT_TRUE(num2 >= num1);
}

TEST(Fp8Test, ComparisonTest) { // NOLINT
    fp8 num1(fp8::from<fp8::Positive, 0b111, 0b0100>());
    fp8 num2(fp8::from<fp8::Positive, 0b111, 0b0010>());
    fp8 num3(fp8::from<fp8::Positive, 0b000, 0b0001>());
    fp8 num4(fp8::from<fp8::Negative, 0b000, 0b0001>());
    fp8 num5(fp8::from<fp8::Negative, 0b011, 0b0010>());
    fp8 num6(fp8::from<fp8::Negative, 0b111, 0b0100>());
    fp8 positiveZero = fp8::getZero();
    fp8 negativeZero(fp8::from<fp8::Negative, 0, 0>());

    EXPECT_TRUE(num1 != num2);
    EXPECT_TRUE(num2 != num3);
    EXPECT_TRUE(num3 != num4);
    EXPECT_TRUE(num3 != num6);

    testEq(num1, num1);

    testGt(num1, num2);
    testGt(num2, num3);
    testGt(num3, num4);
    testGt(num4, num5);
    testGt(num5, num6);

    testGt(num1, num3);
    testGt(num2, num4);
    testGt(num3, num6);
    testGt(num1, num6);

    testGt(num1, positiveZero);
    testGt(num1, fp8::getMin());
    testGt(fp8::getMax(), positiveZero);
    testGt(fp8::getMax(), fp8::getMin());

    // +0 == -0
    testEq(positiveZero, negativeZero);
    // -0 == +0
    testEq(negativeZero, positiveZero);
    // -0 == -0
    testEq(negativeZero, negativeZero);
    // +0 == +0
    testEq(positiveZero, positiveZero);
}

TEST(Fp8Test, ComparisonTest_2p) { // NOLINT
    fp8 num1(fp8::from<fp8::Positive, 0b011, 0b1100>());
    fp8 num2(fp8::from<fp8::Positive, 0b011, 0b0110>());
    fp8 num3(fp8::from<fp8::Positive, 0b011, 0b0010>());
    fp8 num4(fp8::from<fp8::Positive, 0b001, 0b1100>());
    fp8 num5(fp8::from<fp8::Positive, 0b001, 0b0110>());

    EXPECT_TRUE(num1 > num2);
    EXPECT_TRUE(num2 >= num3);
    EXPECT_TRUE(num1 > num3);
    EXPECT_TRUE(num1 > num4);
    EXPECT_TRUE(num1 > num5);
    EXPECT_TRUE(num1 >= num3);
    EXPECT_TRUE(num1 >= num4);
    EXPECT_TRUE(num1 >= num5);
    EXPECT_TRUE(num2 > num4);
    EXPECT_TRUE(num2 > num5);
    EXPECT_TRUE(num2 >= num4);
    EXPECT_TRUE(num2 >= num5);
}

TEST(Fp8Test, ComparisonTest_2n) { // NOLINT
    fp8 num1(fp8::from<fp8::Negative, 0b011, 0b1100>());
    fp8 num2(fp8::from<fp8::Negative, 0b011, 0b0110>());
    fp8 num3(fp8::from<fp8::Negative, 0b011, 0b0010>());
    fp8 num4(fp8::from<fp8::Negative, 0b001, 0b1100>());
    fp8 num5(fp8::from<fp8::Negative, 0b001, 0b0110>());

    EXPECT_TRUE(num1 < num2);
    EXPECT_TRUE(num2 <= num3);
    EXPECT_TRUE(num1 < num3);
    EXPECT_TRUE(num1 < num4);
    EXPECT_TRUE(num1 < num5);
    EXPECT_TRUE(num1 <= num3);
    EXPECT_TRUE(num1 <= num4);
    EXPECT_TRUE(num1 <= num5);
    EXPECT_TRUE(num2 < num4);
    EXPECT_TRUE(num2 < num5);
    EXPECT_TRUE(num2 <= num4);
    EXPECT_TRUE(num2 <= num5);
}
