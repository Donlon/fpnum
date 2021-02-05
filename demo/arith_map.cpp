#include <iostream>

#include <fpnum.h>
#include <fp8.h>
#include <vector>

#include "common.h"

enum OpType {
    Add,
    Mul
};

template<typename _fpType, OpType opType>
void arithTest(MatFile &matFile, const char *name) {
    const int w = 1 << (_fpType::exp_n + _fpType::man_n + 1);
    std::vector<_fpType> row;
    row.reserve(w);
    for (size_t e = _fpType::exp_max + 1; e-- > 0;) {
        for (size_t m = _fpType::man_max + 1; m-- > 0;) {
            row.push_back({_fpType::Negative, e, m});
        }
    }
    for (size_t e = 0; e <= _fpType::exp_max; e++) {
        for (size_t m = 0; m <= _fpType::man_max; m++) {
            row.push_back({_fpType::Positive, e, m});
        }
    }
    std::vector<_fpType> col = row;

    std::vector<std::vector<_fpType>> map;
    std::vector<std::vector<float>> errorMap;
    map.resize(w);
    errorMap.resize(w);

    for (auto &v : map) {
        v.resize(w);
    }
    for (auto &v : errorMap) {
        v.resize(w);
    }

    float maxAbsError = std::numeric_limits<float>::min();
    float errorSum = 0.;
    float absErrorSum = 0.;

    for (size_t i = 0; i < w; i++) {
        auto x = row[i];
        auto xv = (float) x;
        for (size_t j = 0; j < w; j++) {
            auto y = col[j];
            auto yv = (float) y;
            auto &res = map[i][j];
            float expectedRes;
            if constexpr (opType == Add) {
                res = x + y;
                expectedRes = xv + yv;
            } else if constexpr (opType == Mul) {
                res = x * y;
                expectedRes = xv * yv;
            }

            float error;
            if (std::abs(expectedRes) < _fpType::getPositiveMin().getValue()) {
                error = 0.f;
            } else {
                error = (float) res / expectedRes - 1.f;
            }
            errorMap[i][j] = error;
            float absErr = std::abs(error);
            if (absErr > maxAbsError) {
                maxAbsError = absErr;
            }
            errorSum += error;
            absErrorSum += absErr;
        }
    }

    std::cout << "=========== " << name << "=========== " << std::endl;
    std::cout << "Max abs error:     " << maxAbsError << std::endl;
    std::cout << "Average error:     " << errorSum / (w * w) << std::endl;
    std::cout << "Average abs error: " << absErrorSum / (w * w) << std::endl;

#if defined(MATLAB) && defined(EXPORT_MAT)
    matFile.matAppendData(std::string("x_") + name, row);
    matFile.matAppendData(std::string("y_") + name, col);
    matFile.matAppendData2d(std::string("error_") + name, errorMap);
#endif

    std::cout << std::endl;
}

int main() {
#if defined(MATLAB) && defined(EXPORT_MAT)
    MatFile matFile("arith_map.mat", "w");
    if (!matFile.isOpen()) {
        std::cerr << "Can't open accuracy.mat for writing" << std::endl;
        return 1;
    }
#endif

    arithTest<fp8a, Add>(matFile, "fp8add");
    arithTest<fp8, Mul>(matFile, "fp8mul");

}
