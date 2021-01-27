#pragma once

/// @brief Absolute range: 0.5078125 ~ 1.984375
using fp8y = fpnum<1, 6>;

/// @brief Absolute range: 0.12597656 ~ 1.9921875 (15.81x)
using fp8z = fpnum<2, 7>;

/// @brief Absolute range: 0.00830078125 ~ 1.9375 (233.43x)
using fp8 = fpnum<3, 4>;

/// @brief Absolute range: 3.4332275e-05 ~ 1.875 (54613.33x)
using fp8a = fpnum<4, 3>;

/// @brief Absolute range: 5.8207661e-10 ~ 1.75 (3.01e9x)
using fp8b = fpnum<5, 2>;

/// @brief
using fp8c = fpnum<6, 1>;
