#pragma once

#include <cstddef>
#include <utility>
#include <vector>
#include "common/serialization/serialization_compat.h"

namespace MikageSerialization {

template <class Archive, class T>
void serialize(Archive& ar, std::vector<T>& value, const unsigned int file_version) {
    split_free(ar, value, file_version);
}

template <class Archive, class T>
void save(Archive& ar, const std::vector<T>& value, const unsigned int) {
    std::size_t size = value.size();
    ar & size;
    for (const auto& entry : value) {
        auto copy = entry;
        ar & copy;
    }
}

template <class Archive, class T>
void load(Archive& ar, std::vector<T>& value, const unsigned int) {
    std::size_t size{};
    ar & size;
    value.clear();
    value.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        T entry{};
        ar & entry;
        value.emplace_back(std::move(entry));
    }
}

} // namespace MikageSerialization
