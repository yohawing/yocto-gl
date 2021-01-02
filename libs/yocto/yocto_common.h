//
// # Yocto/Common: Common utilities
//
// Yocto/Common is a collection of utilities helpful in implementing other
// Yocto/GL libraries. Yocto/Common is implemented in `yocto_common.h`.
//

//
// LICENSE:
//
// Copyright (c) 2016 -- 2021 Fabio Pellacini
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//

#ifndef _YOCTO_COMMON_H_
#define _YOCTO_COMMON_H_

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

#include <tuple>
#include <utility>

// -----------------------------------------------------------------------------
// USING DIRECTIVES
// -----------------------------------------------------------------------------
namespace yocto {

// No using directives yet

}  // namespace yocto

// -----------------------------------------------------------------------------
// PYTHON-LIKE ITERATORS
// -----------------------------------------------------------------------------
namespace yocto {

// Python `range()` equivalent. Construct an object that iterates over an
// integer sequence.
template <typename T>
constexpr auto range(T max);
template <typename T>
constexpr auto range(T min, T max);
template <typename T>
constexpr auto range(T min, T max, T step);

// Python `enumerate()` equivalent. Construct an object that iterates over a
// sequence of elements and numbers them.
template <typename T,
    typename Iterator = decltype(std::begin(std::declval<T>())),
    typename          = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T&& iterable);

}  // namespace yocto

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PYTHON-LIKE ITERATORS
// -----------------------------------------------------------------------------
namespace yocto {

// Python `range()` equivalent. Construct an object to iterate over a sequence.
template <typename T>
constexpr auto range(T max) {
  return range((T)0, max, (T)1);
}
template <typename T>
constexpr auto range(T min, T max) {
  return range(min, max, (T)1);
}
template <typename T>
constexpr auto range(T min, T max, T step) {
  struct iterator {
    T    index;
    void operator++() { ++index; }
    bool operator!=(const iterator& other) const {
      return index != other.index;
    }
    T operator*() const { return index; }
  };
  struct range_helper {
    T        begin_ = 0, end_ = 0;
    iterator begin() const { return {begin_}; }
    iterator end() const { return {end_}; }
  };
  return range_helper{min, max};
}

// Python `enumerate()` equivalent. Construct an object that iteraterates over a
// sequence of elements and numbers them.
// Implementation from http://reedbeta.com/blog/python-like-enumerate-in-cpp17/
// template <typename T,
//    typename Iterator = decltype(std::begin(std::declval<T>())),
//    typename          = decltype(std::end(std::declval<T>()))>
// constexpr auto enumerate(T&& iterable) {
//  // maybe we should avoid tuples here
//  struct iterator {
//    std::size_t index;
//    Iterator    iter;
//    bool operator!=(const iterator& other) const { return iter != other.iter;
//    } void operator++() {
//      ++index;
//      ++iter;
//    }
//    auto operator*() const { return std::tie(index, *iter); }
//  };
//  struct iterable_wrapper {
//    T    iterable;
//    auto begin() { return iterator{0, std::begin(iterable)}; }
//    auto end() { return iterator{0, std::end(iterable)}; }
//  };
//  return iterable_wrapper{std::forward<T>(iterable)};
//}

template <typename T>
struct view {
  T*     _data = nullptr;
  size_t _count = 0;

    view() = default;
    
    view(T* ptr, size_t count) {
      _data  = ptr;
      _count = count;
    }
  view(const view<T>& vec) {
    _data  = vec._data;
    _count = vec._count;
  }
  view(const std::vector<T>& vec) {
    _data  = (T*)vec.data();
    _count = vec.size();
  }
  view(std::vector<T>& vec) {
    _data  = vec.data();
    _count = vec.size();
  }

  inline T&       operator[](size_t i) { return _data[i]; }
  inline const T& operator[](size_t i) const { return _data[i]; }

  // std:: vector interface
  inline size_t   size() const { return _count; }
  inline bool     empty() const { return _count == 0; }
  inline T*       data() const { return _data; }
  inline T*       begin() { return _data; }
  inline T*       end() { return _data + _count; }
  inline const T* begin() const { return _data; }
  inline const T* end() const { return _data + _count; }
  inline const T& front() const { return _data[0]; }
  inline T&       front() { return _data[0]; }
  inline const T& back() const { return _data[_count - 1]; }
  inline T&       back() { return _data[_count - 1]; }

  using value_type = T;

  // inline void     insert(const T* pos, const T* from, const T* to) {
  //   assert(0 && "not supported yet");
  // }
};

template <typename T>
inline std::vector<T> copy(const view<T>& v) {
  return std::vector<T>(v.begin(), v.end());
}

}  // namespace yocto

#endif
