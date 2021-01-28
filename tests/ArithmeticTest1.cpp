#include <fpnum.h>
#include <fp8.h>
#include <gtest/gtest.h>

TEST(Fp8OppositTest, _0) { // NOLINT
    fp8 positiveZero = fp8::getZero();
    fp8 negativeZero(fp8::from<fp8::Negative, 0, 0>());
    EXPECT_EQ(-positiveZero, positiveZero);
    EXPECT_EQ(-negativeZero, negativeZero);
    fp8 num1(fp8::from<fp8::Positive, 0b111, 0b00>());
    EXPECT_EQ(-num1, (fp8::from<fp8::Negative, 0b111, 0b00>()));
    fp8 num2(fp8::Negative, 0b011, 0b1100);
    EXPECT_EQ(-num2, (fp8::from<fp8::Positive, 0b011, 0b1100>()));
    fp8 num3(fp8::Positive, 0b111, 0b1111);
    EXPECT_EQ(-num3, (fp8::from<fp8::Negative, 0b111, 0b1111>()));
}

TEST(Fp8AdditionTest, Zeros_p) { // NOLINT
    fp8 positiveZero = fp8::getZero();
    fp8 negativeZero(fp8::from<fp8::Negative, 0, 0>());
    EXPECT_EQ(positiveZero + positiveZero, positiveZero);
    EXPECT_EQ(positiveZero + negativeZero, positiveZero);

    fp8 num1(fp8::from<fp8::Positive, 0b111, 0b1000>());
    fp8 num2(fp8::from<fp8::Positive, 0b111, 0b1010>());
    fp8 num3(fp8::from<fp8::Positive, 0b010, 0b0110>());
    fp8 num4(fp8::from<fp8::Positive, 0b000, 0b0110>());

    EXPECT_EQ(positiveZero + num1, num1);
    EXPECT_EQ(num1 + positiveZero, num1);
    EXPECT_EQ(negativeZero + num1, num1);
    EXPECT_EQ(num1 + negativeZero, num1);

    EXPECT_EQ(positiveZero + num2, num2);
    EXPECT_EQ(num2 + positiveZero, num2);
    EXPECT_EQ(negativeZero + num2, num2);
    EXPECT_EQ(num2 + negativeZero, num2);

    EXPECT_EQ(positiveZero + num3, num3);
    EXPECT_EQ(num3 + positiveZero, num3);
    EXPECT_EQ(negativeZero + num3, num3);
    EXPECT_EQ(num3 + negativeZero, num3);

    EXPECT_EQ(positiveZero + num4, num4);
    EXPECT_EQ(num4 + positiveZero, num4);
    EXPECT_EQ(negativeZero + num4, num4);
    EXPECT_EQ(num4 + negativeZero, num4);
}

TEST(Fp8AdditionTest, Zeros_n) { // NOLINT
    fp8 positiveZero = fp8::getZero();
    fp8 negativeZero(fp8::from<fp8::Negative, 0, 0>());

    fp8 num1(fp8::from<fp8::Negative, 0b111, 0b1000>());
    fp8 num2(fp8::from<fp8::Negative, 0b111, 0b1010>());
    fp8 num3(fp8::from<fp8::Negative, 0b010, 0b0110>());
    fp8 num4(fp8::from<fp8::Negative, 0b000, 0b0110>());

    EXPECT_EQ(positiveZero + num1, num1);
    EXPECT_EQ(num1 + positiveZero, num1);
    EXPECT_EQ(negativeZero + num1, num1);
    EXPECT_EQ(num1 + negativeZero, num1);

    EXPECT_EQ(positiveZero + num2, num2);
    EXPECT_EQ(num2 + positiveZero, num2);
    EXPECT_EQ(negativeZero + num2, num2);
    EXPECT_EQ(num2 + negativeZero, num2);

    EXPECT_EQ(positiveZero + num3, num3);
    EXPECT_EQ(num3 + positiveZero, num3);
    EXPECT_EQ(negativeZero + num3, num3);
    EXPECT_EQ(num3 + negativeZero, num3);

    EXPECT_EQ(positiveZero + num4, num4);
    EXPECT_EQ(num4 + positiveZero, num4);
    EXPECT_EQ(negativeZero + num4, num4);
    EXPECT_EQ(num4 + negativeZero, num4);
}

TEST(Fp8AdditionTest, Overflows) { // NOLINT
    EXPECT_EQ((fp8::getMax() + fp8::getMax()), fp8::getMax());
    EXPECT_EQ((fp8::getMin() + fp8::getMin()), fp8::getMin());

    // 1_0000e7 + 1_0000e7 : overflows
    EXPECT_EQ((fp8::positiveNum<7, 0>() + fp8::positiveNum<7, 0>()), fp8::getMax());
    // 1_0011e7 + 1_1110e6 : overflows
    EXPECT_EQ((fp8::positiveNum<7, 3>() + fp8::positiveNum<6, 15>()), fp8::getMax());
    // 1_0110e7 + 1_1110e6 : overflows
    EXPECT_EQ((fp8::positiveNum<7, 3>() + fp8::positiveNum<6, 15>()), fp8::getMax());
}

TEST(Fp8AdditionTest, pp) { // NOLINT
    // 1_0000e1 + 1_0000e1 = 10_0000e1 = 1_0000e2
    EXPECT_EQ((fp8::positiveNum<1, 0>() + fp8::positiveNum<1, 0>()), (fp8::positiveNum<2, 0>()));
    // 1_0000e6 + 1_0000e6 = 10_0000e6 = 1_0000e7
    EXPECT_EQ((fp8::positiveNum<6, 0>() + fp8::positiveNum<6, 0>()), (fp8::positiveNum<7, 0>()));
    // 1_0000e5 + 1_0000e5 = 10_0000e5 = 1_0000e6
    EXPECT_EQ((fp8::positiveNum<5, 0>() + fp8::positiveNum<5, 0>()), (fp8::positiveNum<6, 0>()));
    // 1_1111e5 + 1_1111e5 = 11_1110e5 = 1_1111e6
    EXPECT_EQ((fp8::positiveNum<5, 15>() + fp8::positiveNum<5, 15>()), (fp8::positiveNum<6, 15>()));
    // 1_0011e6 + 1_1001e6 = 10_1100e6 = 1_0110e7
    EXPECT_EQ((fp8::positiveNum<6, 3>() + fp8::positiveNum<6, 9>()), (fp8::positiveNum<7, 6>()));
    // 1_0000e2 + 1_0001e0 = 1_0100e2
    EXPECT_EQ((fp8::positiveNum<2, 0>() + fp8::positiveNum<0, 1>()), (fp8::positiveNum<2, 4>()));
    EXPECT_EQ((fp8::positiveNum<0, 1>() + fp8::positiveNum<2, 0>()), (fp8::positiveNum<2, 4>()));
    // 1_0000e2 + 1_1111e0 = 1_0111e2
    EXPECT_EQ((fp8::positiveNum<2, 0>() + fp8::positiveNum<0, 15>()), (fp8::positiveNum<2, 7>()));
    EXPECT_EQ((fp8::positiveNum<0, 15>() + fp8::positiveNum<2, 0>()), (fp8::positiveNum<2, 7>()));
    // 1_0000e5 + 1_1111e0 = 1_0000e5
    EXPECT_EQ((fp8::positiveNum<5, 0>() + fp8::positiveNum<0, 15>()), (fp8::positiveNum<5, 0>()));
    EXPECT_EQ((fp8::positiveNum<0, 15>() + fp8::positiveNum<5, 0>()), (fp8::positiveNum<5, 0>()));
    // 1_0101e7 + 1_1111e3 = 1_0110e7
    EXPECT_EQ((fp8::positiveNum<7, 5>() + fp8::positiveNum<3, 15>()), (fp8::positiveNum<7, 6>()));
    EXPECT_EQ((fp8::positiveNum<3, 15>() + fp8::positiveNum<7, 5>()), (fp8::positiveNum<7, 6>()));
    // 1_0101e7 + 1_1111e4 = 1_1000e7
    EXPECT_EQ((fp8::positiveNum<7, 5>() + fp8::positiveNum<4, 15>()), (fp8::positiveNum<7, 8>()));
    EXPECT_EQ((fp8::positiveNum<4, 15>() + fp8::positiveNum<7, 5>()), (fp8::positiveNum<7, 8>()));

    // 1_0000e7 + 1_1111e6 : max
    EXPECT_EQ((fp8::positiveNum<7, 0>() + fp8::positiveNum<6, 15>()), fp8::getMax());
    // 1_0000e7 + 1_1100e6 : not max
    EXPECT_EQ((fp8::positiveNum<7, 0>() + fp8::positiveNum<6, 0b1100>()),
              (fp8::getMax() - fp8::positiveNum<3, 0>()));
    // 1_0000e7 + 1_1000e6 : 1_1100e7
    EXPECT_EQ((fp8::positiveNum<7, 0>() + fp8::positiveNum<6, 0b1000>()),
              (fp8::getMax() - fp8::positiveNum<4, 0b1000>()));
}

TEST(Fp8AdditionTest, _nn) { // NOLINT
}

TEST(Fp8SubtractionTest, _0) { // NOLINT
    EXPECT_EQ((fp8::getMax() + fp8::getMin()), fp8::getZero());
    EXPECT_EQ((fp8::positiveNum<3, 5>() + fp8::negativeNum<3, 5>()), fp8::getZero());

    EXPECT_EQ((fp8::positiveNum<1, 0>() + fp8::negativeNum<1, 0>()), fp8::getZero());
    EXPECT_EQ((fp8::positiveNum<5, 0>() + fp8::negativeNum<5, 0>()), fp8::getZero());
    EXPECT_EQ((fp8::positiveNum<7, 13>() + fp8::negativeNum<7, 13>()), fp8::getZero());

    // 1_0101e3 - 1_0100e1 = 1_0000e3
    EXPECT_EQ((fp8::positiveNum<3, 5>() + fp8::negativeNum<1, 4>()), (fp8::positiveNum<3, 0>()));
    EXPECT_EQ((fp8::positiveNum<1, 4>() + fp8::negativeNum<3, 5>()), (fp8::negativeNum<3, 0>()));
    EXPECT_EQ((fp8::negativeNum<1, 4>() + fp8::positiveNum<3, 5>()), (fp8::positiveNum<3, 0>()));
    EXPECT_EQ((fp8::negativeNum<3, 5>() + fp8::positiveNum<1, 4>()), (fp8::negativeNum<3, 0>()));
    // 1_0101e5 - 1_0000e1 = 1_0100e5
    EXPECT_EQ((fp8::positiveNum<5, 5>() + fp8::negativeNum<1, 0>()), (fp8::positiveNum<5, 4>()));
    EXPECT_EQ((fp8::positiveNum<1, 0>() + fp8::negativeNum<5, 5>()), (fp8::negativeNum<5, 4>()));
    EXPECT_EQ((fp8::negativeNum<1, 0>() + fp8::positiveNum<5, 5>()), (fp8::positiveNum<5, 4>()));
    EXPECT_EQ((fp8::negativeNum<5, 5>() + fp8::positiveNum<1, 0>()), (fp8::negativeNum<5, 4>()));
    // 1_0111e7 - 1_1100e5 = 1_0000e7
    EXPECT_EQ((fp8::positiveNum<7, 7>() + fp8::negativeNum<5, 12>()), (fp8::positiveNum<7, 0>()));
    EXPECT_EQ((fp8::positiveNum<5, 12>() + fp8::negativeNum<7, 7>()), (fp8::negativeNum<7, 0>()));
    EXPECT_EQ((fp8::negativeNum<5, 12>() + fp8::positiveNum<7, 7>()), (fp8::positiveNum<7, 0>()));
    EXPECT_EQ((fp8::negativeNum<7, 7>() + fp8::positiveNum<5, 12>()), (fp8::negativeNum<7, 0>()));
    // 1_0000e4 - 1_0000e3 = 0_1000e4 = 1_0000e3
    EXPECT_EQ((fp8::positiveNum<4, 0>() + fp8::negativeNum<3, 0>()), (fp8::positiveNum<3, 0>()));
    EXPECT_EQ((fp8::positiveNum<3, 0>() + fp8::negativeNum<4, 0>()), (fp8::negativeNum<3, 0>()));
    // 1_0000e4 - 1_1000e3 = 0_0100e4 = 1_0000e2
    EXPECT_EQ((fp8::positiveNum<4, 0>() + fp8::negativeNum<3, 8>()), (fp8::positiveNum<2, 0>()));
    EXPECT_EQ((fp8::positiveNum<3, 8>() + fp8::negativeNum<4, 0>()), (fp8::negativeNum<2, 0>()));
    // 1_0000e4 - 1_1100e3 = 0_0010e4 = 1_0000e1
    EXPECT_EQ((fp8::positiveNum<4, 0>() + fp8::negativeNum<3, 12>()), (fp8::positiveNum<1, 0>()));
    EXPECT_EQ((fp8::positiveNum<3, 12>() + fp8::negativeNum<4, 0>()), (fp8::negativeNum<1, 0>()));
    // 1_1000e7 - 1_0011e7 = 0_0101e7 = 1_0100e5
    EXPECT_EQ((fp8::positiveNum<7, 8>() + fp8::negativeNum<7, 3>()), (fp8::positiveNum<5, 4>()));
    EXPECT_EQ((fp8::positiveNum<7, 3>() + fp8::negativeNum<7, 8>()), (fp8::negativeNum<5, 4>()));
    // 1_0010e5 - 1_0000e2 = 1_0000e5
    EXPECT_EQ((fp8::positiveNum<5, 2>() + fp8::negativeNum<2, 0>()), (fp8::positiveNum<5, 0>()));
    EXPECT_EQ((fp8::positiveNum<2, 0>() + fp8::negativeNum<5, 2>()), (fp8::negativeNum<5, 0>()));
}
