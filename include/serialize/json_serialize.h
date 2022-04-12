#pragma once
#pragma once

#include "config.h"
#include "serializable.h"
#include "tools.h"
#include "types.h"

#include <type_traits>

#include <nlohmann/json.hpp>

namespace proplib
{

  namespace serdes
  {
    template <>
    void begin_map<nlohmann::json>(nlohmann::json& node)
    {
      // node << YAML::BeginMap;
    }

    template <>
    void end_map<nlohmann::json>(nlohmann::json& node)
    {
      // node << YAML::EndMap;
    }

    template <class T>
    res_t serialize_field_other(const std::string& key, T* val, const std::string& logger_id, const std::string& doc_string, const bool& scheme,
                                nlohmann::json& node, ...)
    {
      if (!val)
        return res_t::error;

      node[key] = *val;
      if (scheme && !doc_string.empty())
      {
        node[key + "_doc"] = doc_string;
      }
      return res_t::ok;
    }

    template <class T>
    res_t serialize_field_other(const std::string& key, T* val, const std::string& logger_id, const std::string& doc_string, const bool& scheme,
                                nlohmann::json& node, Serializable*)
    {
      nlohmann::json other;
      val->set_logger(logger_id);
      auto res = val->serialize(other, true);
      if (res != res_t::ok)
        return res;

      node[key] = other;

      if (scheme && !doc_string.empty())
      {
        node[key + "_doc"] = doc_string;
      }

      return res_t::ok;
    }

    template <class T>
    typename std::enable_if<std::is_base_of<Serializable, typename clear_type<T>::type>::value, res_t>::type
    serialize_field_vector_serializable(const std::string& key, std::vector<T>* val, const std::string& logger_id, const std::string& doc_string,
                                        const bool& scheme, nlohmann::json& node)
    {
      if (!val)
        return res_t::error;

      for (auto& serval : *val)
      {
        nlohmann::json other;
        serval->set_logger(logger_id);
        auto res = serval->serialize(other, true);
        if (res != res_t::ok)
          return res;
        node[key].push_back(other);
      }

      if (scheme && !doc_string.empty())
      {
        node[key + "_doc"] = doc_string;
      }

      return res_t::ok;
    }

    template <class T>
    typename std::enable_if<(!std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    serialize_field_vector_serializable(const std::string& key, std::vector<T>* val, const std::string& logger_id, const std::string& doc_string,
                                        const bool& scheme, nlohmann::json& node)
    {
      return serialize_field_other(key, val, logger_id, doc_string, scheme, node);
    }

    template <class K, class T>
    typename std::enable_if<(!std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    serialize_field_map_serializable(const std::string& key, std::map<K, T>* val, const std::string& logger_id, const std::string& doc_string,
                                     const bool& scheme, nlohmann::json& node)
    {
      return serialize_field_other(key, val, logger_id, doc_string, scheme, node);
    }

    template <class K, class T>
    typename std::enable_if<(std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    serialize_field_map_serializable(const std::string& key, std::map<K, T>* val, const std::string& logger_id, const std::string& doc_string,
                                     const bool& scheme, nlohmann::json& node)
    {
      if (!val)
        return res_t::error;

      nlohmann::json otherMap;

      for (auto& serval : *val)
      {
        nlohmann::json otherVal;
        auto           res = serval.second->serialize(otherVal, true);

        otherMap[serval.first] = otherVal;
      }
      node[key] = otherMap;
      if (scheme && !doc_string.empty())
      {
        node[key + "_doc"] = doc_string;
      }

      return res_t::ok;
    }

    template <class T>
    typename std::enable_if<is_vector<T>::value, res_t>::type serialize_field(const std::string& key, T* val, const std::string& logger_id,
                                                                              const std::string& doc_string, const bool& scheme, nlohmann::json& node)
    {
      return serialize_field_vector_serializable(key, val, logger_id, doc_string, scheme, node);
    }

    template <class T>
    typename std::enable_if<is_map<T>::value, res_t>::type serialize_field(const std::string& key, T* val, const std::string& logger_id,
                                                                           std::string doc_string, const bool& scheme, nlohmann::json& node)
    {
      return serialize_field_map_serializable(key, val, logger_id, doc_string, scheme, node);
    }

    template <class T>
    typename std::enable_if<(!is_stl_container<T>::value), res_t>::type serialize_field(const std::string& key, T* val, const std::string& logger_id,
                                                                                        const std::string& doc_string, const bool& scheme,
                                                                                        nlohmann::json& node)
    {
      return serialize_field_other(key, val, logger_id, doc_string, scheme, node, (clear_type_v<decltype(val)>*)nullptr);
    }

    //! =============================deserialization===============================

    template <class T>
    res_t deserialize_field_other(const std::string& key, T* val, const std::string& logger_id, const nlohmann::json& node, ...)
    {
      if (!node.contains(key))
      {
#if ENABLE_SERDES_LOGGING
        CLOG(WARNING, logger_id.c_str()) << "no key "
                                         << "\"" << key << "\".";
#endif
        return res_t::key_not_found;
      }
      try
      {
        *val = node[key].get<T>();
        return res_t::ok;
      }
      catch (nlohmann::json::type_error& e)
      {
#if ENABLE_SERDES_LOGGING
        CLOG(ERROR, logger_id.c_str()) << "failed to deserialize key "
                                       << "\"" << key << "\"."
                                       << " reason: " << e.what();
#endif
        return res_t::error;
      }
      return res_t::error;
    }

    template <class T>
    res_t deserialize_field_other(const std::string& key, T* val, const std::string& logger_id, const nlohmann::json& node, Serializable*)
    {
      if (!node.contains(key))
      {
#if ENABLE_SERDES_LOGGING
        CLOG(WARNING, logger_id.c_str()) << "no key "
                                         << "\"" << key << "\".";
#endif
        return res_t::key_not_found;
      }

      val->set_logger(logger_id);
      return val->deserialize(node[key]);
    }

    template <class T>
    typename std::enable_if<std::is_base_of<Serializable, typename clear_type<T>::type>::value, res_t>::type
    deserialize_field_vector_serializable(const std::string& key, std::vector<T>* val, const std::string& logger_id, const nlohmann::json& node)
    {
      if (!node.contains(key))
      {
#if ENABLE_SERDES_LOGGING
        CLOG(WARNING, logger_id.c_str()) << "no key "
                                         << "\"" << key << "\".";
#endif
        return res_t::key_not_found;
      }
      try
      {
        std::vector<nlohmann::json> nodes = node[key].get<std::vector<nlohmann::json>>();
        val->clear();

        stat_t stat;

        for (auto n : nodes)
        {
          T s = create_var<T>::create();
          call_as_ptr<T>::ptr(s)->set_logger(logger_id);
          res_t res = call_as_ptr<T>::ptr(s)->deserialize(n);
          if (res == res_t::error)
          {
#if ENABLE_SERDES_LOGGING
            CLOG(ERROR, logger_id.c_str()) << "failed to deserialize key "
                                           << "\"" << key << "\".";
#endif
            return res;
          }

          if (res == res_t::all_skiped)
            stat.all_skiped_cnt++;
          else if (res == res_t::key_not_found)
            stat.key_not_found_cnt++;
          else if (res == res_t::not_all_deser)
            stat.not_all_deser_cnt++;

          val->push_back(s);
        }

        auto totall_cnt = nodes.size();
        if (stat.all_skiped_cnt == totall_cnt || stat.key_not_found_cnt == totall_cnt)
          return res_t::all_skiped;
        else if (stat.all_skiped_cnt > 0 || stat.key_not_found_cnt > 0 || stat.not_all_deser_cnt)
          return res_t::not_all_deser;

        return res_t::ok;
      }
      catch (nlohmann::json::type_error& e)
      {
#if ENABLE_SERDES_LOGGING
        CLOG(ERROR, logger_id.c_str()) << "failed to deserialize key "
                                       << "\"" << key << "\"."
                                       << " reason: " << e.what();
#endif
        return res_t::error;
      }
    }

    template <class T>
    typename std::enable_if<(!std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    deserialize_field_vector_serializable(const std::string& key, std::vector<T>* val, const std::string& logger_id, const nlohmann::json& node)
    {
      return deserialize_field_other(key, val, logger_id, node);
    }

    template <class K, class T>
    typename std::enable_if<(!std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    deserialize_field_map_serializable(const std::string& key, std::map<K, T>* val, const std::string& logger_id, const nlohmann::json& node)
    {
      return deserialize_field_other(key, val, logger_id, node);
    }

    template <class K, class T>
    typename std::enable_if<(std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    deserialize_field_map_serializable(const std::string& key, std::map<K, T>* val, const std::string& logger_id, const nlohmann::json& node)
    {

      if (!node.contains(key))
      {
#if ENABLE_SERDES_LOGGING
        CLOG(WARNING, logger_id.c_str()) << "no key "
                                         << "\"" << key << "\".";
#endif
        return res_t::key_not_found;
      }

      try
      {
        nlohmann::json childs = node[key].get<nlohmann::json>();

        stat_t stat;
        for (auto it = childs.begin(); it != childs.end(); ++it)
        {
          std::string key  = it.key();
          auto        nval = it.value();

          res_t res = res_t::ok;
          if (val->count(key))
          {
            auto s = (*val)[key];
            s->set_logger(logger_id);
            res = s->deserialize(childs[key]);
          }
          else
          {
            auto cn = new_derived_serializable<T>();
            if (!cn)
            {
#if ENABLE_SERDES_LOGGING
              CLOG(ERROR, logger_id.c_str()) << "can't create object with type " << typeid(T).name() << "key: "
                                             << "\"" << key << "\".";
#endif
              return res_t::error;
            }
            cn->set_logger(logger_id);
            (*val)[key] = cn;
            res         = cn->deserialize(childs[key]);
          }
          if (res == res_t::error)
          {
#if ENABLE_SERDES_LOGGING
            CLOG(ERROR, logger_id.c_str()) << "failed to deserialize key "
                                           << "\"" << key << "\".";
#endif
            return res;
          }

          if (res == res_t::all_skiped)
            stat.all_skiped_cnt++;
          else if (res == res_t::key_not_found)
            stat.key_not_found_cnt++;
          else if (res == res_t::not_all_deser)
            stat.not_all_deser_cnt++;

          auto totall_cnt = childs.size();
          if (stat.all_skiped_cnt == totall_cnt || stat.key_not_found_cnt == totall_cnt)
            return res_t::all_skiped;
          else if (stat.all_skiped_cnt > 0 || stat.key_not_found_cnt > 0 || stat.not_all_deser_cnt)
            return res_t::not_all_deser;
        }

        return res_t::ok;
      }
      catch (nlohmann::json::type_error& e)
      {
#if ENABLE_SERDES_LOGGING
        CLOG(ERROR, logger_id.c_str()) << "failed to deserialize key "
                                       << "\"" << key << "\"."
                                       << " reason: " << e.what();
#endif
        return res_t::error;
      }
    }
    template <class T>
    typename std::enable_if<is_map<T>::value, res_t>::type deserialize_field(const std::string& key, T* val, const std::string& logger_id,
                                                                             const nlohmann::json& node)
    {
      return deserialize_field_map_serializable(key, val, logger_id, node);
    }
    template <class T>
    typename std::enable_if<is_vector<T>::value, res_t>::type deserialize_field(const std::string& key, T* val, const std::string& logger_id,
                                                                                const nlohmann::json& node)
    {
      return deserialize_field_vector_serializable(key, val, logger_id, node);
    }

    template <class T>
    typename std::enable_if<(!is_stl_container<T>::value), res_t>::type deserialize_field(const std::string& key, T* val,
                                                                                          const std::string& logger_id, const nlohmann::json& node)
    {
      return deserialize_field_other(key, val, logger_id, node, (clear_type_v<decltype(val)>*)nullptr);
    }
  } // namespace serdes
} // namespace proplib
