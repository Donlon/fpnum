#include <fpnum.h>
#include <fp8.h>
#include <gtest/gtest.h>

template<size_t e, size_t m>
void expectPositive(const fpnum<e, m> &num) {
    EXPECT_FALSE(num.isNegative());
    EXPECT_TRUE(num.isPositive());
    EXPECT_FALSE(num.isZero());
}

template<size_t e, size_t m>
void expectNegative(const fpnum<e, m> &num) {
    EXPECT_TRUE(num.isNegative());
    EXPECT_FALSE(num.isPositive());
    EXPECT_FALSE(num.isZero());
}

TEST(Fp8Test, SignTest) { // NOLINT
    fp8 num1(fp8::from<fp8::Positive, 0b111, 0b00>());
    expectPositive(num1);

    fp8 num2(fp8::from<fp8::Negative, 0b111, 0b00>());
    expectNegative(num2);

    fp8 num3(fp8::from<fp8::Positive, 0b000, 0b01>());
    expectPositive(num3);

    fp8 num4(fp8::from<fp8::Negative, 0b000, 0b01>());
    expectNegative(num4);

    fp8 negativeZero(fp8::Negative, 0, 0);
    EXPECT_FALSE(negativeZero.isPositive());
    EXPECT_FALSE(negativeZero.isNegative());
    EXPECT_TRUE(negativeZero.isZero());
    EXPECT_FALSE(fp8::getZero().isPositive());
    EXPECT_FALSE(fp8::getZero().isNegative());
    EXPECT_TRUE(fp8::getZero().isZero());
}
