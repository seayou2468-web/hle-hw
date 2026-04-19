#pragma once

#include <cstddef>
#include <set>
#include <utility>
#include "../common_types.h"
#include "serialization_compat.h"

namespace MikageSerialization {

template <class Archive, class T>
void serialize(Archive& ar, std::set<T>& set, const unsigned int file_version) {
    split_free(ar, set, file_version);
}

template <class Archive, class T>
void save(Archive& ar, const std::set<T>& set, const unsigned int) {
    std::size_t size = set.size();
    ar & size;
    for (const auto& entry : set) {
        auto copy = entry;
        ar & copy;
    }
}

template <class Archive, class T>
void load(Archive& ar, std::set<T>& set, const unsigned int) {
    std::size_t size{};
    ar & size;
    set.clear();
    for (std::size_t i = 0; i < size; ++i) {
        T entry{};
        ar & entry;
        set.emplace(std::move(entry));
    }
}

} // namespace MikageSerialization
