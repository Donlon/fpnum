#include <iostream>

#include <fpnum.h>
#include <fp8.h>
#include <memory>

#include "common.h"

using fp_t = fp8a;

const int kCount = 5000;

template<typename _fp_type>
double getError(double num) {
    _fp_type v((float) num);
    double val = v.getValue();
    if (val == 0.) {
        return 0.;
    }
    return val / num - 1.;
}

template<typename _fp_type>
void testAccuracy(MatFile &matFile, const char *typeName) {
    const int halfCount = kCount / 2;
    const double p = std::pow(0.5, (double) (fp_t::exp_max + 1) / halfCount);

    double num = fp_t::getMax().getValue();
    double maxAbsError = std::numeric_limits<double>::min();
    double errorSum = 0.;
    double absErrorSum = 0.;

    std::unique_ptr<double[]> xValue(new double[kCount]);
    std::unique_ptr<double[]> error(new double[kCount]);

    for (size_t i = 0; i < halfCount; i++) {
        xValue[i] = num;
        double err = getError<_fp_type>(num);
        double absErr = std::abs(err);
        if (absErr > maxAbsError) {
            maxAbsError = absErr;
        }
        error[i] = err;
        errorSum += err;
        absErrorSum += absErr;
        num *= p;
    }

    num = fp_t::getMin().getValue();

    for (size_t i = 0; i < halfCount; i++) {
        xValue[halfCount + i] = num;
        double err = getError<_fp_type>(num);
        double absErr = std::abs(err);
        if (absErr > maxAbsError) {
            maxAbsError = absErr;
        }
        error[halfCount + i] = err;
        errorSum += err;
        absErrorSum += absErr;
        num *= p;
    }

    std::cout << "============= " << typeName << " =============" << std::endl;
    std::cout << "Max abs error:     " << maxAbsError << std::endl;
    std::cout << "Average error:     " << errorSum / kCount << std::endl;
    std::cout << "Average abs error: " << absErrorSum / kCount << std::endl;
    std::cout << std::endl;

    printBuffer(xValue.get(), kCount, "nums");
    printBuffer(error.get(), kCount, "error");
    matFile.matAppendData(std::string("x_") + typeName, xValue.get(), kCount);
    matFile.matAppendData(std::string("error_") + typeName, error.get(), kCount);
}

int main() {
#if defined(MATLAB) && defined(EXPORT_MAT)
    MatFile matFile("accuracy.mat", "w");
    if (!matFile.isOpen()) {
        std::cerr << "Can't open accuracy.mat for writing" << std::endl;
        return 1;
    }
#endif

    testAccuracy<fp8y>(matFile, "fp8y");
    testAccuracy<fp8z>(matFile, "fp8z");
    testAccuracy<fp8>(matFile, "fp8");
    testAccuracy<fp8a>(matFile, "fp8a");
    testAccuracy<fp8b>(matFile, "fp8b");
    testAccuracy<fp8c>(matFile, "fp8c");
}
