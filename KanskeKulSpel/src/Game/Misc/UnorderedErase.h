#pragma once
#include <vector>

template <typename Vector>
void unordered_erase(Vector& v, typename Vector::iterator it) {
    *it = std::move(v.back());
    v.pop_back();
}