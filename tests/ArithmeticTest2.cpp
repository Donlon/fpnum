#include <fpnum.h>
#include <fp8.h>
#include <gtest/gtest.h>

TEST(Fp8MultiplyTest, Zeros) { // NOLINT
    fp8 num1(fp8::from<fp8::Negative, 0b111, 0b1000>());
    fp8 num2(fp8::from<fp8::Negative, 0b111, 0b1010>());
    fp8 num3(fp8::from<fp8::Negative, 0b010, 0b0110>());
    fp8 num4(fp8::from<fp8::Positive, 0b111, 0b1000>());
    fp8 num5(fp8::from<fp8::Positive, 0b111, 0b1010>());
    fp8 num6(fp8::from<fp8::Positive, 0b010, 0b0110>());
    fp8 positiveZero = fp8::getZero();
    fp8 negativeZero(fp8::from<fp8::Negative, 0, 0>());

    EXPECT_EQ(num1 * positiveZero, positiveZero);
    EXPECT_EQ(num2 * positiveZero, positiveZero);
    EXPECT_EQ(num3 * positiveZero, positiveZero);
    EXPECT_EQ(num4 * positiveZero, positiveZero);
    EXPECT_EQ(num5 * positiveZero, positiveZero);
    EXPECT_EQ(num6 * positiveZero, positiveZero);

    EXPECT_EQ(num1 * negativeZero, positiveZero);
    EXPECT_EQ(num2 * negativeZero, positiveZero);
    EXPECT_EQ(num3 * negativeZero, positiveZero);
    EXPECT_EQ(num4 * negativeZero, positiveZero);
    EXPECT_EQ(num5 * negativeZero, positiveZero);
    EXPECT_EQ(num6 * negativeZero, positiveZero);
}

TEST(Fp8MultiplyTest, eq1) { // NOLINT
    auto one = fp8::positiveNum<7, 0>();
    auto negativeOne = -one;

    EXPECT_EQ((fp8::positiveNum<0, 1>() * one), (fp8::positiveNum<0, 1>()));
    EXPECT_EQ((fp8::positiveNum<3, 0>() * one), (fp8::positiveNum<3, 0>()));
    EXPECT_EQ((fp8::positiveNum<5, 15>() * one), (fp8::positiveNum<5, 15>()));
    EXPECT_EQ((fp8::positiveNum<7, 15>() * one), (fp8::positiveNum<7, 15>()));

    EXPECT_EQ((fp8::negativeNum<0, 1>() * negativeOne), (fp8::positiveNum<0, 1>()));
    EXPECT_EQ((fp8::negativeNum<3, 0>() * negativeOne), (fp8::positiveNum<3, 0>()));
    EXPECT_EQ((fp8::negativeNum<5, 15>() * negativeOne), (fp8::positiveNum<5, 15>()));
    EXPECT_EQ((fp8::negativeNum<7, 15>() * negativeOne), (fp8::positiveNum<7, 15>()));
}
