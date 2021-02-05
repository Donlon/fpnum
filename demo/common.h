#pragma once

#include <vector>
#include <cstring>

#ifdef MATLAB

// #include <MatlabEngine.hpp>

#ifdef EXPORT_MAT

#include <mat.h>

#endif
#endif

template<typename>
struct is_fpnum : std::false_type {
};

template<size_t _e, size_t _m, size_t _b>
struct is_fpnum<fpnum<_e, _m, _b>> : std::true_type {
};

template<typename _type>
void printBuffer(_type *data, size_t count, const std::string &label = {}) {
    if (!label.empty()) {
        std::cout << label << "=[";
    }
    for (size_t i = 0; i < count; i++) {
        if constexpr (is_fpnum<_type>::value) {
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

template<typename _type>
void printBuffer(std::vector<_type> &data, const std::string &label = {}) {
    printBuffer(data.data(), data.size(), label);
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

template<typename _type>
bool matAppendData(MATFile *matFile, const std::string &name, const _type *data, size_t size) {
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
    if (!matPutVariable(matFile, name.c_str(), arr)) {
        return false;
    }
    mxDestroyArray(arr);
    return true;
}

template<typename _type>
bool matAppendData2d(MATFile *matFile, const std::string &name, const std::vector<std::vector<_type>> &data) {
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
    if (matPutVariable(matFile, name.c_str(), arr) != 0) {
        return false;
    }
    mxDestroyArray(arr);
    return true;
}

template<typename _type>
bool matAppendData(MATFile *matFile, const std::string &name, const std::vector<_type> &data) {
    return matAppendData(matFile, name, data.data(), data.size());
}
