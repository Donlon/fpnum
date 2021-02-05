#pragma once

#include <string>
#include <vector>

class MatFile {
    void *matFile = nullptr;

public:
    MatFile(const char *filename, const char *mode);

    ~MatFile();

    template<typename _type>
    bool matAppendData(const std::string &name, const _type *data, size_t size);

    template<typename _type>
    bool matAppendData2d(const std::string &name, const std::vector<std::vector<_type>> &data);

    template<typename _type>
    bool matAppendData(const std::string &name, const std::vector<_type> &data) {
        return matAppendData(name, data.data(), data.size());
    }

    bool isOpen();
};
