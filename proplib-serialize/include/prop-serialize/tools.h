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
#include <string>
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
    typedef T type;
  };

  template <class T>
  struct clear_type<T*>
  {
    typedef T type;
  };

  template <class T>
  struct clear_type<T&>
  {
    typedef T type;
  };

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

  std::string std_vector_typename(...) { return std::string("is_not_vector_type"); }

  template <class T>
  constexpr std::string std_vector_typename(std::vector<T> v)
  {
    std::string val = "unknown";
    if (std::is_arithmetic<T>::value)
      val = typeid(T).name();
    else if (std::is_same<T, std::string>::value)
      val = "string";
    return val;
  }

  std::string std_map_typename(...) { return std::string("is_not_map_type"); }

  template <class T, class V>
  constexpr std::string std_map_typename(std::map<T, V> v)
  {
    std::string key = "unknown";
    if (std::is_arithmetic<T>::value)
      key = typeid(T).name();
    else if (std::is_same<T, std::string>::value)
      key = "string";

    std::string val = "unknown";
    if (std::is_arithmetic<V>::value)
      val = typeid(V).name();
    else if (std::is_same<V, std::string>::value)
      val = "string";

    return key + std::string(":") + val;
  }

  template <class T>
  constexpr std::string type_name()
  {
    std::string name = "unknown";

    if (std::is_arithmetic<T>::value)
      name = typeid(T).name();
    else if (std::is_same<T, std::string>::value)
      name = "string";
    else if (is_vector<T>::value)
      name = "vector:" + std_vector_typename(T());
    else if (is_map<T>::value)
      name = "map:" + std_map_typename(T());

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

} // namespace proplib