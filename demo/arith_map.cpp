#include <iostream>

#include <fpnum.h>
#include <fp8.h>
#include <vector>

enum OpType {
    Add,
    Mul
};

template<typename>
struct is_fpnum : std::false_type {
};

template<size_t _e, size_t _m, size_t _b>
struct is_fpnum<fpnum<_e, _m, _b>> : std::true_type {
};

template<typename _type>
void printBuffer(std::vector<_type> &data, const std::string &label = {}) {
    if (!label.empty()) {
        std::cout << label << "=[";
    }
    for (auto it = data.begin(); it != data.end(); it++) {
        if constexpr (is_fpnum<_type>::value) {
            std::cout << it->getValue();
        } else {
            std::cout << *it;
        }
        if (it != data.end() - 1) {
            std::cout << ", ";
        }
    }
    if (!label.empty()) {
        std::cout << "];";
    }
    std::cout << std::endl;
}

template<typename _type>
void printBuffer2d(std::vector<std::vector<_type>> &data, const std::string &label = {}) {
    if (!label.empty()) {
        std::cout << label << "=[";
    }
    for (auto it = data.begin(); it != data.end(); it++) {
        auto row = *it;
        for (auto it2 = row.begin(); it2 != row.end(); it2++) {
            if constexpr (is_fpnum<_type>::value) {
                std::cout << it2->getValue();
            } else {
                std::cout << *it2;
            }
            if (it2 != row.end() - 1) {
                std::cout << ", ";
            }
        }
        if (it != data.end() - 1) {
            std::cout << ";";
        }
    }
    if (!label.empty()) {
        std::cout << "];";
    }
    std::cout << std::endl;
}

template<typename _fpType, OpType opType>
void arithTest() {
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

    float maxAbsError = std::numeric_limits<double>::min();
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
            } else if constexpr(opType == Mul) {
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

    std::cout << "Max abs error:     " << maxAbsError << std::endl;
    std::cout << "Average error:     " << errorSum / (w * w) << std::endl;
    std::cout << "Average abs error: " << absErrorSum / (w * w) << std::endl;

    printBuffer(row, "row");
    printBuffer(col, "col");
    printBuffer2d(errorMap, "error_map");

    std::cout << std::endl;
}

int main() {
    // arithTest<fp8a, Add>();
    arithTest<fp8, Mul>();
}
