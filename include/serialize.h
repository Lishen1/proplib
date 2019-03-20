#pragma once

#include "serializable.h"
#include "serializers/yaml_serialize.h"
#include "tools.h"
#include <functional>
#include <map>

namespace proplib
{
  template <class T>
  Container<T>* try_cast(IContainer* icont)
  {
    return dynamic_cast<Container<clear_type<T>::type>*>(icont);
  }

  template <class T>
  Container<T>* try_const_cast(IContainer* icont)
  {
    return dynamic_cast<Container<clear_type<T>::const_type>*>(icont);
  }

  template <class Ser, class T>
  res_t try_serialize(const std::string& key, IContainer* cont, const std::string& logger_id, const std::string& doc_string, const bool& scheme,
    T& field)
  {
    auto container = try_cast<Ser>(cont);
    if (container)
      return proplib::serdes::serialize_field(key, &field, logger_id, doc_string, scheme, container->node());
    else
      return res_t::error;
  }

  template <class Deser, class T>
  res_t try_deserialize(const std::string& key, IContainer* cont, const std::string& logger_id, T& field)
  {
    auto container = try_cast<Deser>(cont);
    auto const_container = try_const_cast<const Deser>(cont);

    if (const_container)
      return proplib::serdes::deserialize_field(key, &field, logger_id, const_container->node());
    else if (container)
      return proplib::serdes::deserialize_field(key, &field, logger_id, container->node());
    else
      return res_t::error;
  }

  template <class T>
  res_t try_serialize_all(const std::string& key, IContainer* cont, const std::string& logger_id, const std::string& doc_string, const bool& scheme,
    T& field)
  {
    return try_serialize<YAML::Emitter>(key, cont, logger_id, doc_string, scheme, field);
  }

  template <class T>
  res_t try_deserialize_all(const std::string& key, IContainer* cont, const std::string& logger_id, T& field)
  {
    return try_deserialize<YAML::Node>(key, cont, logger_id, field);
  }

#define UNNAMED_IMPL(x, y) UNNAMED_##x##_##y
#define UNNAMED_DECL(x, y) UNNAMED_IMPL(x, y)
#define UNNAMED UNNAMED_DECL(__LINE__, __COUNTER__)

#define SERIALIZE(x, ...)                                                                                  \
  char UNNAMED = add_serdes_lambda(                                                                        \
      #x,                                                                                                  \
      [this](const std::string& key, proplib::IContainer* cont) {                                          \
        return try_serialize_all(key, cont, _logger_id, proplib::get_doc_string(__VA_ARGS__), _scheme, x); \
      },                                                                                                   \
                                                                                                           \
      [this](const std::string& key, proplib::IContainer* cont) mutable { return try_deserialize_all(key, cont, _logger_id, x); })

#define SERIALIZE_SUBS()                                                                                                                             \
  char UNNAMED = serialize_subs([this]() {                                                                                                           \
    for (auto ch : subprops)                                                                                                                         \
    {                                                                                                                                                \
      add_serdes_lambda(                                                                                                                             \
          ch.first,                                                                                                                                  \
          [this, ch](const std::string& key, proplib::IContainer* cont) {                                                                            \
            return try_serialize_all(key, cont, _logger_id, proplib::get_doc_string(), _scheme, *ch.second);                                         \
          },                                                                                                                                         \
          [this, ch](const std::string& key, proplib::IContainer* cont) mutable { return try_deserialize_all(key, cont, _logger_id, *ch.second); }); \
    }                                                                                                                                                \
  })

} // namespace proplib
