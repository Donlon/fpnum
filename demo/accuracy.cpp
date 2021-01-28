#include <iostream>

#include <fpnum.h>
#include <fp8.h>
#include <memory>

using fp_t = fp8a;

const int kCount = 5000;

template<typename _type>
void printBuffer(_type *data, size_t count, const std::string &label = {}) {
    if (!label.empty()) {
        std::cout << label << "=[";
    }
    for (size_t i = 0; i < count; i++) {
        if constexpr (std::is_same<_type, fp_t>::value) {
            std::cout << data[i].getValue();
        } else {
            std::cout << data[i];
        }
        if (i != count - 1) {
            std::cout << ", ";
        }
    }
    if (!label.empty()) {
        std::cout << "];";
    }
    std::cout << std::endl;
}

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
void testAccuracy(const char *typeName) {
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
}

int main() {
    testAccuracy<fp8y>("fp8y");
    testAccuracy<fp8z>("fp8z");
    testAccuracy<fp8>("fp8");
    testAccuracy<fp8a>("fp8a");
    testAccuracy<fp8b>("fp8b");
    testAccuracy<fp8c>("fp8c");
}
