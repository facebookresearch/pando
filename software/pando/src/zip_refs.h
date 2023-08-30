#pragma once

#include <iterator>
#include <tuple>
#include <vector>

namespace pnd {
namespace pando {

namespace detail {
// Recursive base case
template <class T>
void IncAll(T& t) {
  ++t;
}

/** Recursive template function that pre-increments any number of objects. */
template <class T1, class... TN>
void IncAll(T1& t1, TN&... tn) {
  IncAll(t1);
  IncAll(tn...);
}
} // namespace detail

/** Zip together multiple ranges by traversing them and storing references to their elements.
 * The length of the first range (ie. the distance between first1 and last1) determines the length
 * of the result. The second through Nth ranges must all be at least as long as the first one.
 * @param first1 Iterator pointing to the the begining of the first range
 * @param last1 Iterator pointing to the end of the first range
 * @param firstn Iterators pointing to the second through Nth ranges
 *
 * @return A vector of tuples of references, where the kth tuple contains a (possibly const)
 * reference to the kth element in each input range.
 */
template <class InputIt1, class... InputItN>
auto ZipRefs(InputIt1 first1, InputIt1 last1, InputItN... firstn) {
  using RefTuple = std::tuple<
      typename std::iterator_traits<InputIt1>::reference,
      typename std::iterator_traits<InputItN>::reference...>;
  std::vector<RefTuple> zipped_refs;
  zipped_refs.reserve(std::distance(first1, last1));

  while (first1 != last1) {
    zipped_refs.push_back(std::tie(*first1, *firstn...));
    detail::IncAll(first1, firstn...);
  }

  return zipped_refs;
}

} // namespace pando
} // namespace pnd
