#pragma once

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
#include "common/serialization/serialization_compat.h"

namespace MikageSerialization {

template <class Archive, class... Types>
void serialize(Archive& ar, std::variant<Types...>& value, const unsigned int file_version) {
    split_free(ar, value, file_version);
}

template <class Archive, class... Types>
void save(Archive& ar, const std::variant<Types...>& value, const unsigned int) {
    std::size_t index = value.index();
    ar & index;
    std::visit(
        [&](const auto& active) {
            using ActiveT = std::decay_t<decltype(active)>;
            ActiveT copy = active;
            ar & copy;
        },
        value);
}

template <std::size_t I = 0, class Archive, class... Types>
void LoadVariantByIndex(Archive& ar, std::variant<Types...>& value, std::size_t index) {
    if constexpr (I < sizeof...(Types)) {
        if (index == I) {
            using EntryT = std::variant_alternative_t<I, std::variant<Types...>>;
            EntryT entry{};
            ar & entry;
            value = std::move(entry);
            return;
        }
        LoadVariantByIndex<I + 1>(ar, value, index);
    } else {
        throw std::runtime_error("Invalid serialized variant index");
    }
}

template <class Archive, class... Types>
void load(Archive& ar, std::variant<Types...>& value, const unsigned int) {
    std::size_t index{};
    ar & index;
    LoadVariantByIndex(ar, value, index);
}

} // namespace MikageSerialization
