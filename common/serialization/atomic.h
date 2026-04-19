#pragma once

#include <atomic>
#include "serialization_compat.h"

namespace MikageSerialization {

template <class Archive, class T>
void serialize(Archive& ar, std::atomic<T>& value, const unsigned int file_version) {
    split_free(ar, value, file_version);
}

template <class Archive, class T>
void save(Archive& ar, const std::atomic<T>& value, const unsigned int) {
    T tmp = value.load(std::memory_order_relaxed);
    ar & tmp;
}

template <class Archive, class T>
void load(Archive& ar, std::atomic<T>& value, const unsigned int) {
    T tmp{};
    ar & tmp;
    value.store(tmp, std::memory_order_relaxed);
}

} // namespace MikageSerialization
