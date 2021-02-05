#include "MatFile.h"
#include <cstring>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <mat.h>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include <fpnum.h>
#include <fp8.h>

MatFile::MatFile(const char *filename, const char *mode) {
    matFile = matOpen(filename, mode);
    if (!matFile) {
        return;
    }
}

MatFile::~MatFile() {
    matClose(reinterpret_cast<MATFile *>(matFile));
}

bool MatFile::isOpen() {
    return matFile != nullptr;
}

template<typename _type>
bool MatFile::matAppendData(const std::string &name, const _type *data, size_t size) {
    mxArray *arr = mxCreateNumericMatrix(1, size, mxSINGLE_CLASS, mxREAL);
    if (!arr) {
        return false;
    }
    auto *dataPtr = reinterpret_cast<float *>(mxGetPr(arr));
    if constexpr (std::is_same<_type, float>::value) {
        memcpy(dataPtr, data, size * sizeof(float));
    } else {
        for (size_t i = 0; i < size; i++) {
            dataPtr[i] = (float) data[i];
        }
    }
    if (!matPutVariable(reinterpret_cast<MATFile *>(matFile), name.c_str(), arr)) {
        return false;
    }
    mxDestroyArray(arr);
    return true;
}

template<typename _type>
bool MatFile::matAppendData2d(const std::string &name, const std::vector<std::vector<_type>> &data) {
    if (data.empty()) {
        return false;
    }
    mwSize rowSize = data[0].size();
    mxArray *arr = mxCreateNumericMatrix(data.size(), rowSize, mxSINGLE_CLASS, mxREAL);
    if (!arr) {
        return false;
    }
    auto *dataPtr = reinterpret_cast<float *>(mxGetPr(arr));
    for (auto &row : data) {
        assert(row.size() == rowSize);
        if constexpr (std::is_same<_type, float>::value) {
            memcpy(dataPtr, row.data(), rowSize * sizeof(float));
        } else {
            for (size_t i = 0; i < rowSize; i++) {
                dataPtr[i] = (double) row[i];
            }
        }
        dataPtr += rowSize;
    }
    if (matPutVariable(reinterpret_cast<MATFile *>(matFile), name.c_str(), arr) != 0) {
        return false;
    }
    mxDestroyArray(arr);
    return true;
}

// Dirty workaround

// matAppendData
template
bool MatFile::matAppendData(const std::string &name, const fp8y *data, size_t size);

template
bool MatFile::matAppendData(const std::string &name, const fp8z *data, size_t size);

template
bool MatFile::matAppendData(const std::string &name, const fp8 *data, size_t size);

template
bool MatFile::matAppendData(const std::string &name, const fp8a *data, size_t size);

template
bool MatFile::matAppendData(const std::string &name, const fp8b *data, size_t size);

template
bool MatFile::matAppendData(const std::string &name, const float *data, size_t size);

template
bool MatFile::matAppendData(const std::string &name, const double *data, size_t size);

// matAppendData2d
template
bool MatFile::matAppendData2d(const std::string &name, const std::vector<std::vector<fp8a>> &data);

template
bool MatFile::matAppendData2d(const std::string &name, const std::vector<std::vector<fp8>> &data);

template
bool MatFile::matAppendData2d(const std::string &name, const std::vector<std::vector<float>> &data);
