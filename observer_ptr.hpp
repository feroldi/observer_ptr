// MIT License
//
// Copyright (c) 2017 Mário Feroldi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// observer_ptr (Library Fundamentals TS v2)
// http://en.cppreference.com/w/cpp/experimental/observer_ptr

#pragma once

#include <cstddef>
#include <type_traits>
#include <functional>

namespace sunfyre
{

template <typename T>
struct observer_ptr
{
  using element_type = T;

  constexpr observer_ptr() noexcept : ptr(nullptr) {};
  constexpr observer_ptr(std::nullptr_t) noexcept : ptr(nullptr) {};
  constexpr explicit observer_ptr(element_type* p) noexcept : ptr(p) {}

  template <
    typename W2,
    std::enable_if_t<std::is_convertible<W2*, element_type*>::value>...>
  observer_ptr(observer_ptr<W2> other) noexcept : ptr(other.get()) {}

  observer_ptr(const observer_ptr&) = default;
  observer_ptr(observer_ptr&&) = default;

  /// Modifiers

  constexpr auto release() noexcept -> element_type*
  {
    element_type* p = this->get();
    this->reset();
    return p;
  }

  constexpr void reset(element_type* p = nullptr) noexcept { this->ptr = p; }

  constexpr void swap(observer_ptr& other) noexcept
  {
    using std::swap;
    swap(this->ptr, other.ptr);
  }

  /// Observers

  constexpr auto get() const noexcept -> element_type* { return this->ptr; }

  constexpr operator bool() const noexcept { return this->get() != nullptr; }

  constexpr auto operator* () const noexcept
    -> std::add_lvalue_reference_t<element_type>
  { return *this->get(); }

  constexpr auto operator-> () const noexcept
    -> element_type*
  { return this->get(); }

  constexpr explicit operator element_type* () const noexcept
  { return this->get(); }


private:
  T* ptr;
};

template <typename T>
inline auto make_observer(T* p) noexcept
{ return ::sunfyre::observer_ptr<T>(p); }

template <typename W1, typename W2>
inline bool operator== (const ::sunfyre::observer_ptr<W1>& p1, const ::sunfyre::observer_ptr<W2>& p2)
{ return p1.get() == p2.get(); }

template <typename W1, typename W2>
inline bool operator!= (const ::sunfyre::observer_ptr<W1>& p1, const ::sunfyre::observer_ptr<W2>& p2)
{ return !(p1 == p2); }

template <typename W>
inline bool operator== (const ::sunfyre::observer_ptr<W>& p, std::nullptr_t) noexcept
{ return !p; }

template <typename W>
inline bool operator== (std::nullptr_t, const ::sunfyre::observer_ptr<W>& p) noexcept
{ return !p; }

template <typename W>
inline bool operator!= (const ::sunfyre::observer_ptr<W>& p, std::nullptr_t) noexcept
{ return static_cast<bool>(p); }

template <typename W>
inline bool operator!= (std::nullptr_t, const ::sunfyre::observer_ptr<W>& p) noexcept
{ return static_cast<bool>(p); }

template <typename W1, typename W2>
inline bool operator< (const ::sunfyre::observer_ptr<W1>& p1, const ::sunfyre::observer_ptr<W2>& p2)
{
  using W3 = std::common_type_t<W1*, W2*>;
  return std::less<W3>()(p1.get(), p2.get());
}

template <typename W1, typename W2>
inline bool operator> (const ::sunfyre::observer_ptr<W1>& p1, const ::sunfyre::observer_ptr<W2>& p2)
{ return p2 < p1; }

template <typename W1, typename W2>
inline bool operator<= (const ::sunfyre::observer_ptr<W1>& p1, const ::sunfyre::observer_ptr<W2>& p2)
{ return !(p2 < p1); }

template <typename W1, typename W2>
inline bool operator>= (const ::sunfyre::observer_ptr<W1>& p1, const ::sunfyre::observer_ptr<W2>& p2)
{ return !(p1 < p2); }

} // namespace sunfyre

namespace std
{
template <typename W>
inline void swap(::sunfyre::observer_ptr<W>& lhs, ::sunfyre::observer_ptr<W>& rhs) noexcept
{ lhs.swap(rhs); }

template <typename T>
struct hash<::sunfyre::observer_ptr<T>>
{
  hash() noexcept = default;

  constexpr auto operator() (::sunfyre::observer_ptr<T> p) noexcept -> std::size_t
  {
    return std::hash<T*>()(p.get());
  }
};
} // namespace std
