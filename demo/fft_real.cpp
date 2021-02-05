#include <iostream>
#include <memory>

#include <fpnum.h>
#include <fp8.h>
#include <codecvt>

#include "common.h"

const size_t fftSize_ldn = 8; // fft256
const size_t fftSize = 1 << fftSize_ldn;

static size_t figIndex = 1;

template<typename _num_type>
std::unique_ptr<_num_type[]> generateFFTTable() {
    size_t size = fftSize * sizeof(_num_type);
    std::unique_ptr<_num_type[]> table(reinterpret_cast<_num_type *>(operator new(size)));
    for (size_t i = 0; i < fftSize; i++) {
        new(&table[i]) _num_type(-sinf((float) i / (float) fftSize * 2.f * (float) M_PI));
    }
    return table;
}

template<typename _num_type>
std::unique_ptr<_num_type[]> generateSourceData() {
    std::unique_ptr<_num_type[]> data(reinterpret_cast<_num_type *>(operator new(fftSize * sizeof(_num_type))));
    for (size_t i = 0; i < fftSize; i++) {
        new(&data[i]) _num_type(cosf((float) i / (float) fftSize * 53.5f * (float) M_PI));
    }
    return data;
}

template<typename _type>
void permutateBuffer(_type *data, const size_t *cycles, size_t cyclesCount) {
    size_t head;
    _type headVal{};

    bool isHead = true;
    size_t lastPt;
    const size_t *p = cycles;
    const size_t *end = &p[cyclesCount];
    for (; p < end; p++) {
        size_t pt = *p;
        if (isHead) {
            head = pt;
            headVal = data[pt];
            isHead = false;
        } else {
            if (pt == head) {
                data[lastPt] = headVal;
                isHead = true;
            } else {
                data[lastPt] = data[pt];
            }
        }
        lastPt = pt;
    }
}

template<typename _num_type>
std::tuple<_num_type, _num_type> getTwiddleFactor(_num_type *twiddleFactorList, size_t i) {
    return {twiddleFactorList[(i - fftSize / 4 + fftSize) % fftSize],
            twiddleFactorList[i % fftSize]};
}

template<typename _num_type>
std::tuple<_num_type, _num_type> fft2(_num_type x1, _num_type x2) {
    return {x1 / 2 + x2 / 2, x1 / 2 - x2 / 2};
}

template<typename _num_type>
std::tuple<_num_type, _num_type> complexMultiply(_num_type op1_real, _num_type op1_imag,
                                                 _num_type op2_real, _num_type op2_imag) {
    // assert(real <= max);
    // assert(real >= min);
    // assert(imag <= max);
    // assert(imag >= min);
    auto real = op1_real * op2_real - op1_imag * op2_imag;
    auto imag = op1_real * op2_imag + op1_imag * op2_real;
    return {real, imag};
}

// DIT FFT128 radix-4
// FFT steps: 2->4->4->4
const size_t kFFTR4DIT128PermutationChains[] = {
        0x01, 0x40, 0x02, 0x10, 0x08, 0x04, 0x20, 0x01,
        0x03, 0x50, 0x0a, 0x14, 0x28, 0x05, 0x60, 0x03,
        0x06, 0x30, 0x09, 0x44, 0x22, 0x11, 0x48, 0x06,
        0x07, 0x70, 0x0b, 0x54, 0x2a, 0x15, 0x68, 0x07,
        0x0c, 0x24, 0x21, 0x41, 0x42, 0x12, 0x18, 0x0c,
        0x0d, 0x64, 0x23, 0x51, 0x4a, 0x16, 0x38, 0x0d,
        0x0e, 0x34, 0x29, 0x45, 0x62, 0x13, 0x58, 0x0e,
        0x0f, 0x74, 0x2b, 0x55, 0x6a, 0x17, 0x78, 0x0f,
        0x19, 0x4c, 0x26, 0x31, 0x49, 0x46, 0x32, 0x19,
        0x1a, 0x1c, 0x2c, 0x25, 0x61, 0x43, 0x52, 0x1a,
        0x1b, 0x5c, 0x2e, 0x35, 0x69, 0x47, 0x72, 0x1b,
        0x1d, 0x6c, 0x27, 0x71, 0x4b, 0x56, 0x3a, 0x1d,
        0x1e, 0x3c, 0x2d, 0x65, 0x63, 0x53, 0x5a, 0x1e,
        0x1f, 0x7c, 0x2f, 0x75, 0x6b, 0x57, 0x7a, 0x1f,
        0x33, 0x59, 0x4e, 0x36, 0x39, 0x4d, 0x66, 0x33,
        0x37, 0x79, 0x4f, 0x76, 0x3b, 0x5d, 0x6e, 0x37,
        0x3d, 0x6d, 0x67, 0x73, 0x5b, 0x5e, 0x3e, 0x3d,
        0x3f, 0x7d, 0x6f, 0x77, 0x7b, 0x5f, 0x7e, 0x3f,
};
const size_t kFFTR4DIT128PermutationChainsSize = 8 * 18;

template<typename _num_type>
void fftDIT4_128(_num_type *f_real, _num_type *f_imag, _num_type *twiddleFactorList) {
    const int LX = 2;
    const int ldn = 7;
    const int n = 128;

    permutateBuffer(f_real, kFFTR4DIT128PermutationChains, kFFTR4DIT128PermutationChainsSize);
    permutateBuffer(f_imag, kFFTR4DIT128PermutationChains, kFFTR4DIT128PermutationChainsSize);

    std::cout << "After permutation:" << std::endl;
    printBuffer(f_real, fftSize / 2);
    printBuffer(f_imag, fftSize / 2);

    for (size_t r = 0; r < n; r += 2) {
        const size_t i0 = r;
        const size_t i1 = r + 1;
        std::tie(f_real[i0], f_real[i1]) = fft2(f_real[i0], f_real[i1]);
        std::tie(f_imag[i0], f_imag[i1]) = fft2(f_imag[i0], f_imag[i1]);
    }

    std::cout << "After butterfly#1:" << std::endl;
    printBuffer(f_real, fftSize / 2);
    printBuffer(f_imag, fftSize / 2);

    for (size_t ldm = 1 + LX; ldm <= ldn; ldm += LX) {
        const size_t m = 1u << ldm;
        const size_t m4 = m >> LX;

        for (int j = 0; j < (int) m4; j++) {
            int n_div = j * (1 << (7 - ldm));
            const auto[e1_real, e1_imag] = getTwiddleFactor(twiddleFactorList, 2 * (n_div));
            const auto[e2_real, e2_imag] = getTwiddleFactor(twiddleFactorList, 2 * (2 * n_div));
            const auto[e3_real, e3_imag] = getTwiddleFactor(twiddleFactorList, 2 * (3 * n_div));
            // TODO: optimize for situation j = 0

            for (uint r = 0; r < n; r += (1u << ldm)) {
                size_t i0 = j + r;
                size_t i1 = i0 + m4;
                size_t i2 = i0 + m4 * 2;
                size_t i3 = i0 + m4 * 3;

                _num_type a0_real = f_real[i0];
                _num_type a0_imag = f_imag[i0];

                auto[a1_real, a1_imag] = complexMultiply(f_real[i1], f_imag[i1], e1_real, e1_imag);
                auto[a2_real, a2_imag] = complexMultiply(f_real[i2], f_imag[i2], e2_real, e2_imag);
                auto[a3_real, a3_imag] = complexMultiply(f_real[i3], f_imag[i3], e3_real, e3_imag);
                // ai = f[ii] * ei

                auto[u_comm_real, u_diff_real] = fft2(a0_real, a2_real);
                auto[u_comm_imag, u_diff_imag] = fft2(a0_imag, a2_imag);
                auto[v_comm_real, v_diff_real] = fft2(a1_real, a3_real);
                auto[v_comm_imag, v_diff_imag] = fft2(a1_imag, a3_imag);

                std::tie(f_real[i0], f_real[i2]) = fft2(u_comm_real, v_comm_real);
                std::tie(f_imag[i0], f_imag[i2]) = fft2(u_comm_imag, v_comm_imag);
                std::tie(f_real[i1], f_real[i3]) = fft2(u_diff_real, v_diff_imag);
                std::tie(f_imag[i3], f_imag[i1]) = fft2(u_diff_imag, v_diff_real);

                // W1 = e^j(-2pi/m)
                /*
                 *  a0 = f[i0] * W0
                 *  a1 = f[i1] * W1
                 *  a2 = f[i2] * W2
                 *  a3 = f[i3] * W3
                 *
                 *  f[i0] = /1   1   1   1\  / f[i0] * W0 \
                 *  f[i1] = |1  -j  -1   j|  | f[i1] * W1 |
                 *  f[i2] = |1  -1   1  -1|  | f[i2] * W2 |
                 *  f[i3] = \1   j  -1  -j/  \ f[i3] * W3 /
                */
            }
        }

        std::cout << "After butterfly#" << ldm << ":" << std::endl;
        printBuffer(f_real, fftSize / 2);
        printBuffer(f_imag, fftSize / 2);
    }
}

template<typename _num_type>
auto fftRealFp(_num_type *fftSourceData, _num_type *twiddleFactorList) {
    static_assert(fftSize_ldn == 8);

    std::unique_ptr<_num_type[]> dataReal(reinterpret_cast<_num_type *>(operator new(fftSize / 2 * sizeof(_num_type))));
    std::unique_ptr<_num_type[]> dataImag(reinterpret_cast<_num_type *>(operator new(fftSize / 2 * sizeof(_num_type))));
    for (size_t i = 0; i < fftSize; i++) {
        if (!(i % 2)) {
            // even
            dataReal[i / 2] = fftSourceData[i];
        } else {
            // odd
            dataImag[i / 2] = fftSourceData[i];
        }
    }

    fftDIT4_128<_num_type>(dataReal.get(), dataImag.get(), twiddleFactorList);

    dataReal[0] = dataReal[0] + dataImag[0];
    dataImag[0] = _num_type(0);

    const size_t halfSize = fftSize / 2;
    for (size_t k = 1; k < halfSize / 2; k++) {
        auto A_real = (dataReal[k] + dataReal[halfSize - k]);
        auto A_imag = (dataImag[k] - dataImag[halfSize - k]);
        auto B_real = -(dataReal[k] - dataReal[halfSize - k]);
        auto B_imag = -(dataImag[k] + dataImag[halfSize - k]);

        const auto[W_real, W_imag] = getTwiddleFactor(twiddleFactorList, k);
        auto[Bm_real, Bm_imag] = complexMultiply(B_real, B_imag, W_real, W_imag);

        dataReal[k] = A_real / 2 - Bm_imag / 2;
        dataImag[k] = A_imag / 2 + Bm_real / 2;
        dataReal[halfSize - k] = A_real / 2 + Bm_imag / 2;
        dataImag[halfSize - k] = -A_imag / 2 + Bm_real / 2;
    }
    return std::make_pair(std::move(dataReal), std::move(dataImag));
}

template<typename _num_type>
void testFFT(matlab::engine::MATLABEngine &engine, MatFile &matFile, const char *typeName) {
    auto fftTable = generateFFTTable<_num_type>();
    auto sourceData = generateSourceData<_num_type>();
    std::cout << "FFT table:" << std::endl;
    printBuffer(fftTable.get(), fftSize);
    std::cout << "Source data:" << std::endl;
    printBuffer(sourceData.get(), fftSize);

    auto[resultReal, resultImag] = fftRealFp<_num_type>(sourceData.get(), fftTable.get());
    std::unique_ptr<float[]> dataPower(new float[fftSize / 2]);

    std::cout << "Result:" << std::endl;
    printBuffer(resultReal.get(), fftSize / 2);
    printBuffer(resultImag.get(), fftSize / 2);

    for (size_t i = 0; i < fftSize / 2; i++) {
        float real;
        float imag;
        if constexpr (is_fpnum<_num_type>::value) {
            real = (float) resultReal[i].getValue();
            imag = (float) resultImag[i].getValue();
        } else {
            real = (float) resultReal[i];
            imag = (float) resultImag[i];
        }
        dataPower[i] = real * real + imag * imag;
    }

    std::cout << "Output power:" << std::endl;
    printBuffer(dataPower.get(), fftSize / 2, "out");

#if defined(MATLAB) && defined(EXPORT_MAT)
    matFile.matAppendData(std::string("power_") + typeName, dataPower.get(), fftSize / 2);
#endif
#if defined(MATLAB) && defined(SHOW_FIG)
    matlab::data::ArrayFactory factory;
    matlab::data::TypedArray<float> powerArray = factory.createArray({1, fftSize / 2}, dataPower.get(),
                                                                     dataPower.get() + fftSize / 2);
    matlab::data::Array dbArray = engine.feval(u"pow2db", powerArray);
    engine.feval(u"subplot", {
            factory.createScalar<double>(2),
            factory.createScalar<double>(1),
            factory.createScalar<double>(figIndex),
    });
    engine.feval(u"plot", dbArray);
    engine.feval(u"title", {factory.createScalar(typeName)});
    figIndex++;
#endif

    std::cout << std::endl;
    std::cout << std::endl;
}

int main() {
#if defined(MATLAB) && defined(EXPORT_MAT)
    MatFile matFile("fft_real.mat", "w");
    if (!matFile.isOpen()) {
        std::cerr << "Can't open accuracy.mat for writing" << std::endl;
        return 1;
    }
#endif
#if defined(MATLAB) && defined(SHOW_FIG)
    std::vector<std::u16string> matlabs = matlab::engine::findMATLAB();
    if (matlabs.empty()) {
        std::cerr << "No shared MATLAB is found" << std::endl;
        return 1;
    }
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
    for (auto &s : matlabs) {
        std::cout << converter.to_bytes(s) << std::endl;
    }
    std::unique_ptr<matlab::engine::MATLABEngine> matlabPtr = matlab::engine::connectMATLAB(matlabs[0]);
    if (!matlabPtr) {
        std::cerr << "Can't connect to the MATLAB instance" << std::endl;
        return 1;
    }
    matlab::data::ArrayFactory factory;
    matlab::data::CharArray version = matlabPtr->feval(u"version", factory.createEmptyArray());
    std::cout << "Connected to: " << version.toAscii() << std::endl;

    matlabPtr->eval(u"close all");
    matlabPtr->eval(u"figure");
#endif

    testFFT<float>(*matlabPtr, matFile, "float");
    testFFT<fp8a>(*matlabPtr, matFile, "fp8a");
}
