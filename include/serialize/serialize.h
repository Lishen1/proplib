#pragma once

#include "json_serialize.h"
#include "serializable.h"
#include "tools.h"
#include "yaml_serialize.h"
#include <functional>
#include <map>

namespace proplib
{
  template <class T>
  Container<T>* try_cast(IContainer* icont)
  {
    return dynamic_cast<Container<clear_type_v<T>>*>(icont);
  }

  template <class T>
  Container<T>* try_const_cast(IContainer* icont)
  {
    return dynamic_cast<Container<clear_const_type_v<T>>*>(icont);
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
    auto container       = try_cast<Deser>(cont);
    auto const_container = try_const_cast<const Deser>(cont);

    if (const_container)
      return proplib::serdes::deserialize_field(key, &field, logger_id, const_container->node());
    else if (container)
      return proplib::serdes::deserialize_field(key, &field, logger_id, container->node());
    else
      return res_t::error;
  }

  template <class SER, class T>
  res_t try_serialize_all(const std::string& key, IContainer* cont, const std::string& logger_id, const std::string& doc_string, const bool& scheme,
                          T& field)
  {
    return try_serialize<SER>(key, cont, logger_id, doc_string, scheme, field);
  }

  template <class DESER, class T>
  res_t try_deserialize_all(const std::string& key, IContainer* cont, const std::string& logger_id, T& field)
  {
    return try_deserialize<DESER>(key, cont, logger_id, field);
  }

#define UNNAMED_IMPL(x, y) UNNAMED_##x##_##y
#define UNNAMED_DECL(x, y) UNNAMED_IMPL(x, y)
#define UNNAMED UNNAMED_DECL(__LINE__, __COUNTER__)

#define SERIALIZE_IMPL(SER, DESER, x, ...)                                                                                  \
  char UNNAMED = add_serdes_lambda(                                                                                         \
      #x,                                                                                                                   \
      [this](const std::string& key, proplib::IContainer* cont)                                                             \
      { return proplib::try_serialize_all<SER>(key, cont, _logger_id, proplib::get_doc_string(__VA_ARGS__), _scheme, x); }, \
                                                                                                                            \
      [this](const std::string& key, proplib::IContainer* cont) mutable { return proplib::try_deserialize_all<DESER>(key, cont, _logger_id, x); })

#define SERIALIZE(x, ...)                                    \
  SERIALIZE_IMPL(YAML::Emitter, YAML::Node, x, __VA_ARGS__); \
  SERIALIZE_IMPL(nlohmann::json, nlohmann::json, x, __VA_ARGS__)

#define SERIALIZE_SUBS_IMPL(SER, DESER)                                                                                           \
  char UNNAMED = serialize_subs(                                                                                                  \
      [this]()                                                                                                                    \
      {                                                                                                                           \
        for (auto ch : subprops)                                                                                                  \
        {                                                                                                                         \
          add_serdes_lambda(                                                                                                      \
              ch.first,                                                                                                           \
              [this, ch](const std::string& key, proplib::IContainer* cont)                                                       \
              { return proplib::try_serialize_all<SER>(key, cont, _logger_id, proplib::get_doc_string(), _scheme, *ch.second); }, \
              [this, ch](const std::string& key, proplib::IContainer* cont) mutable                                               \
              { return proplib::try_deserialize_all<DESER>(key, cont, _logger_id, *ch.second); });                                \
        }                                                                                                                         \
      })

#define SERIALIZE_SUBS()                          \
  SERIALIZE_SUBS_IMPL(YAML::Emitter, YAML::Node); \
  SERIALIZE_SUBS_IMPL(nlohmann::json, nlohmann::json)

} // namespace proplib
