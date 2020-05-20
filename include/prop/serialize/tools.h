#pragma once

#include <map>
#include <type_traits>
#include <typeinfo>
#include <deque>
#include <forward_list>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace proplib
{
  template <class T>
  struct clear_type
  {
    using type = T;
    using const_type = const T;
  };
  template<typename T>
  using clear_type_v = typename clear_type<T>::type;

  template<typename T>
  using clear_const_type_v = typename clear_type<T>::const_type;

  template <class T>
  struct clear_type<T*>
  {
    typedef T type;
    typedef const T const_type;
  };

  template <class T>
  struct clear_type<T&>
  {
    typedef T type;
    typedef const T const_type;
  };

  //-------------------

  template <class T>
  struct clear_type<const T*>
  {
    typedef T type;
    typedef const T const_type;
  };

  template <class T>
  struct clear_type<const T&>
  {
    typedef T type;
    typedef const T const_type;
  };

  template <class T>
  struct clear_type<const T>
  {
    typedef T type;
    typedef const T const_type;
  };

  //-------------------

  template <typename T>
  struct is_vector : public std::false_type
  {
  };
  template <typename T, typename A>
  struct is_vector<std::vector<T, A>> : public std::true_type
  {
  };

  template <typename T>
  struct is_map : public std::false_type
  {
  };
  template <typename K, typename V>
  struct is_map<std::map<K, V>> : public std::true_type
  {
  };

  template <class T>
  constexpr std::string_view type_name();

//  template< typename ...Args>
//  std::string_view std_vector_typename([[maybe_unused]] Args ...args) { return std::string_view("is_not_vector_type"); }
//
//  template <class T>
//  constexpr std::string_view std_vector_typename( [[maybe_unused]] std::vector<T> v)
//  {
//    return type_name<typename std::vector<T>::value_type>();
//  }
//
//  //inline std::string std_map_typename(...) { return std::string("is_not_map_type"); }
//
//  template <class T, class V>
//  constexpr std::string_view std_map_typename( [[maybe_unused]] std::map<T, V> v)
//  {
//    auto key = type_name<T>();
//    auto val = type_name<V>();
//    return key + std::string(":") + val;
//  }

  template <class T>
  constexpr std::string_view type_name()
  {
    std::string_view name = "unknown";

    if (std::is_arithmetic<T>::value)
    {
      if (std::is_same<T, uint64_t>::value)
        name = "uint64_t";
      else if (std::is_same<T, uint32_t>::value)
        name = "uint32_t";
      else if (std::is_same<T, uint16_t>::value)
        name = "uint16_t";
      else if (std::is_same<T, uint8_t>::value)
        name = "uint8_t";
      else if (std::is_same<T, int64_t>::value)
        name = "int64_t";
      else if (std::is_same<T, int32_t>::value)
        name = "int32_t";
      else if (std::is_same<T, int16_t>::value)
        name = "int16_t";
      else if (std::is_same<T, int8_t>::value)
        name = "int8_t";
      else if (std::is_same<T, double>::value)
        name = "double";
      else if (std::is_same<T, float>::value)
        name = "float";
      else if (std::is_same<T, bool>::value)
        name = "bool";
    }
    else if (std::is_same<T, std::string>::value) {
      name = "string";
    }
    else if (is_vector<T>::value) {
      name = "vector"; //std_vector_typename(std::declval<T>());
    }
    else if (is_map<T>::value) {
      name = "map";// + std_map_typename(std::declval<T>());
    }

    return name;
  }

  //specialize a type for all of the STL containers.
  namespace is_stl_container_impl {
    template <typename T>       struct is_stl_container :std::false_type {};
    template <typename T, std::size_t N> struct is_stl_container<std::array    <T, N>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::vector            <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::deque             <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::list              <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::forward_list      <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::set               <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::multiset          <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::map               <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::multimap          <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::unordered_set     <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::unordered_multiset<Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::unordered_map     <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::unordered_multimap<Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::stack             <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::queue             <Args...>> :std::true_type {};
    template <typename... Args> struct is_stl_container<std::priority_queue    <Args...>> :std::true_type {};
  }

  //type trait to utilize the implementation type traits as well as decay the type
  template <typename T> struct is_stl_container {
    static constexpr bool const value = is_stl_container_impl::is_stl_container<std::decay_t<T>>::value;
  };

  inline std::string get_doc_string()
  {
    return "";
  }

  inline std::string get_doc_string(const char* str)
  {
    return str;
  }

} // namespace proplib
