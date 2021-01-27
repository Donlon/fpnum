#include <fpnum.h>
#include <fp8.h>
#include <gtest/gtest.h>

TEST(Fp8Test, ConversionTest_getValue) { // NOLINT
    EXPECT_FLOAT_EQ(fp8::getZero().getValue(), 0.);
    EXPECT_FLOAT_EQ((fp8::negativeNum<0, 0>().getValue()), 0.);
    // range: -1.9375f - 1.9375f
    EXPECT_FLOAT_EQ(fp8::getMin().getValue(), -1.9375);
    EXPECT_FLOAT_EQ(fp8::getMax().getValue(), 1.9375);

    EXPECT_FLOAT_EQ((fp8::positiveNum<0, 1>().getValue()), 0.00830078125);
    EXPECT_FLOAT_EQ((fp8::negativeNum<0, 1>().getValue()), -0.00830078125);

    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 0>().getValue()), 0.0625);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 1>().getValue()), 0.06640625); // +0.00390625 = 2 ^ -8
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 2>().getValue()), 0.0703125);  // +0.00390625
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 3>().getValue()), 0.07421875); // +0.00390625
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 4>().getValue()), 0.078125);   // ...
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 5>().getValue()), 0.08203125);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 6>().getValue()), 0.0859375);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 7>().getValue()), 0.08984375);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 8>().getValue()), 0.09375);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 9>().getValue()), 0.09765625);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 10>().getValue()), 0.1015625);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 11>().getValue()), 0.10546875);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 12>().getValue()), 0.109375);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 13>().getValue()), 0.11328125);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 14>().getValue()), 0.1171875);
    EXPECT_FLOAT_EQ((fp8::positiveNum<3, 15>().getValue()), 0.12109375);
    EXPECT_FLOAT_EQ((fp8::positiveNum<4, 0>().getValue()), 0.125);       // +0.00390625
    EXPECT_FLOAT_EQ((fp8::positiveNum<4, 0>().getValue()), 0.125);       // +0.00390625

    // other variants
    EXPECT_FLOAT_EQ(fp8y::getMin().getValue(), -1.984375);
    EXPECT_FLOAT_EQ((fp8y::positiveNum<0, 1>().getValue()), 0.5078125);
    EXPECT_FLOAT_EQ(fp8z::getMin().getValue(), -1.9921875);
    EXPECT_FLOAT_EQ((fp8z::positiveNum<0, 1>().getValue()), 0.12597656);
    EXPECT_FLOAT_EQ(fp8a::getMin().getValue(), -1.875);
    EXPECT_FLOAT_EQ((fp8a::positiveNum<0, 1>().getValue()), 3.4332275e-05);
    EXPECT_FLOAT_EQ(fp8b::getMin().getValue(), -1.75);
    EXPECT_FLOAT_EQ((fp8b::positiveNum<0, 1>().getValue()), 5.8207661e-10);
    EXPECT_FLOAT_EQ(fp8c::getMin().getValue(), -1.5);
    EXPECT_FLOAT_EQ((fp8c::positiveNum<0, 1>().getValue()), 1.6263033e-19);
}

TEST(Fp8Test, ConversionTest_fromValue) { // NOLINT
    EXPECT_EQ(fp8::fromValue(0.f), fp8::getZero());
    EXPECT_EQ(fp8::fromValue(1.9375f), fp8::getMax());
    EXPECT_EQ(fp8::fromValue(-1.9375f), fp8::getMin());
    EXPECT_EQ(fp8::fromValue(0.00830078125), (fp8::positiveNum<0, 1>()));

    EXPECT_EQ(fp8::fromValue(0.0625), (fp8::positiveNum<3, 0>()));
    EXPECT_EQ(fp8::fromValue(0.08984375), (fp8::positiveNum<3, 7>()));
    EXPECT_EQ(fp8::fromValue(0.1015625), (fp8::positiveNum<3, 10>()));
    EXPECT_EQ(fp8::fromValue(0.125), (fp8::positiveNum<4, 0>()));
    EXPECT_EQ(fp8::fromValue(0.5), (fp8::positiveNum<6, 0>()));
    EXPECT_EQ(fp8::fromValue(0.453125), (fp8::positiveNum<5, 13>()));

    EXPECT_EQ(fp8::fromValue(0.119140625 - 0.0002), (fp8::positiveNum<3, 14>())); // 3, 14: 0.1171875
    EXPECT_EQ(fp8::fromValue(0.119140625 + 0.0002), (fp8::positiveNum<3, 15>())); // 3, 15: 0.12109375

    EXPECT_EQ(fp8::fromValue(0.123046875 - 0.0002), (fp8::positiveNum<3, 15>())); // 3, 15: 0.12109375
    EXPECT_EQ(fp8::fromValue(0.123046875 + 0.0002), (fp8::positiveNum<4, 0>()));  // 4, 0:  0.125

    EXPECT_EQ(fp8::fromValue(-0.0625), (fp8::negativeNum<3, 0>()));
    EXPECT_EQ(fp8::fromValue(-0.08984375), (fp8::negativeNum<3, 7>()));
    EXPECT_EQ(fp8::fromValue(-0.1015625), (fp8::negativeNum<3, 10>()));
    EXPECT_EQ(fp8::fromValue(-0.125), (fp8::negativeNum<4, 0>()));
    EXPECT_EQ(fp8::fromValue(-0.5), (fp8::negativeNum<6, 0>()));
    EXPECT_EQ(fp8::fromValue(-0.453125), (fp8::negativeNum<5, 13>()));
}

TEST(Fp8Test, ConversionTest_toRepresentation) { // NOLINT
    EXPECT_EQ(fp8::getZero().toRepresentation(), "+0");
    EXPECT_EQ(fp8::getZero().toRepresentation(), "+0");
    EXPECT_EQ(fp8(fp8::Negative, 0, 0).toRepresentation(), "-0");
    EXPECT_EQ((fp8::positiveNum<0b0, 0b1>().toRepresentation()), "+10001e0");
    EXPECT_EQ((fp8::positiveNum<0b11, 0b1010>().toRepresentation()), "+11010e11");
    EXPECT_EQ((fp8::negativeNum<0b11, 0b1010>().toRepresentation()), "-11010e11");
    EXPECT_EQ((fp8::negativeNum<0b111, 0b1000>().toRepresentation()), "-11000e111");
}
 