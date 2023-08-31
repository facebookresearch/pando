#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace pnd {
namespace pando {

/** Value type for KVContainer. */
template <class Key, class T>
using KVPair = std::pair<const Key, T>;

/** Container that contains key-value pairs with unique keys.
Similar to std::map, but does not implement lookup, insertion, or deletion. */
template <class Key, class T>
struct KVContainer : private std::vector<KVPair<Key, T>> {
  // Expose std::vector member types
  using typename KVContainer::vector::const_iterator;
  using typename KVContainer::vector::const_reference;
  using typename KVContainer::vector::difference_type;
  using typename KVContainer::vector::iterator;
  using typename KVContainer::vector::reference;
  using typename KVContainer::vector::size_type;
  using typename KVContainer::vector::value_type;

  /** Construct an empty container */
  KVContainer() = default;

  /** Construct a new container with specified keys and values constructed using specified
   * arguments.
   * @param first_key iterator pointing to the beginning of a range of keys
   * @param last_key  iterator pointing to the end of a range of keys
   * @param args  arguments to be forwarded to the constructor of each contained value
   */
  template <
      class InputIt,
      class... Args,
      typename = std::enable_if_t<
          std::is_same<typename std::iterator_traits<InputIt>::value_type, Key>::value>>
  KVContainer(InputIt first_key, InputIt last_key, const Args&... args) {
    // Sort the keys
    std::vector<Key> keys(std::distance(first_key, last_key));
    std::partial_sort_copy(first_key, last_key, keys.begin(), keys.end());

    // Delete duplicate keys
    auto new_end = std::unique(keys.begin(), keys.end());
    keys.erase(new_end, keys.end());

    // Construct key-value pairs in place
    this->reserve(keys.size());
    for (auto& key : keys) {
      this->emplace_back(
          std::piecewise_construct,
          std::forward_as_tuple(std::move(key)),
          std::forward_as_tuple(args...));
    }
  }

  // Expose selected std::vector member functions
  using KVContainer::vector::begin;
  using KVContainer::vector::cbegin;
  using KVContainer::vector::cend;
  using KVContainer::vector::empty;
  using KVContainer::vector::end;
  using KVContainer::vector::size;
};

} // namespace pando
} // namespace pnd
