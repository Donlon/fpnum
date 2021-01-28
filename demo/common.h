#pragma once

#include <vector>

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
