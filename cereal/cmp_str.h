#ifndef CEREAL_CMP_STR_H
#define CEREAL_CMP_STR_H

#include <cstring>

/**
 * const char* comparison
 * need to research how this memory would be allocated when stored in map
 */
struct cmp_str {
    bool operator()(char const* a, char const* b) const {
        return std::strcmp(a, b) < 0;
    }
};

#endif