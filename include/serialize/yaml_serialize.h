#pragma once

#include "config.h"
#include "serializable.h"
#include "tools.h"
#include "types.h"
//#include <easylogging++.h>
#include <type_traits>
#include <yaml-cpp/yaml.h>

namespace proplib
{

  namespace serdes
  {
    template <>
    static void begin_map<YAML::Emitter>(YAML::Emitter& node)
    {
      node << YAML::BeginMap;
    }

    template <>
    static void end_map<YAML::Emitter>(YAML::Emitter& node)
    {
      node << YAML::EndMap;
    }

    template <class T>
    typename std::enable_if<(!std::is_same<Serializable, typename clear_type<T>::type>::value), typename clear_type<T>::type*>::type
    new_derived_serializable()
    {
      return new typename clear_type<T>::type;
    }

    template <class T>
    typename std::enable_if<(std::is_same<Serializable, typename clear_type<T>::type>::value), typename clear_type<T>::type*>::type
    new_derived_serializable()
    {
      return nullptr;
    }

    template <class T>
    res_t serialize_field_other(const std::string& key, T* val, const std::string& logger_id, const std::string& doc_string, const bool& scheme,
                                YAML::Emitter& node, ...)
    {
      node.SetSeqFormat(YAML::Flow);
      std::string name = type_name<T>();
      node << YAML::Key << key;

      if (scheme)
        node << YAML::VerbatimTag(std::string(name));

      node << YAML::Value << *val;

      if (scheme && doc_string.size())
      {
        node << YAML::Key << (key + "_doc") << YAML::VerbatimTag("doc");
        node << YAML::Value << doc_string;
      }

      if (!node.good())
      {
#if ENABLE_SERDES_LOGGING
        CLOG(ERROR, logger_id.c_str()) << "failed to serialize key "
                                       << "\"" << key << "\"."
                                       << " reason: " << node.GetLastError();
#endif
        return res_t::error;
      }

      return res_t::ok;
    }

    template <class T>
    res_t serialize_field_other(const std::string& key, T* val, const std::string& logger_id, const std::string& doc_string, const bool& scheme,
                                YAML::Emitter& node, Serializable*)
    {
      node << YAML::Key << key;
      if (scheme)
      {
        node << YAML::VerbatimTag("serializable");
      }
      node << YAML::Value << YAML::BeginMap;
      val->set_logger(logger_id);
      if (val->leaf_serialize(node, scheme) != res_t::ok)
      {
#if ENABLE_SERDES_LOGGING
        CLOG(ERROR, logger_id.c_str()) << "failed to serialize key "
                                       << "\"" << key << "\".";
#endif
        return res_t::error;
      }
      node << YAML::EndMap;

      if (scheme && doc_string.size())
      {
        node << YAML::Key << (key + "_doc") << YAML::VerbatimTag("doc");
        node << YAML::Value << doc_string;
      }

      if (!node.good())
      {
#if ENABLE_SERDES_LOGGING
        CLOG(ERROR, logger_id.c_str()) << "failed to serialize key "
                                       << "\"" << key << "\"."
                                       << " reason: " << node.GetLastError();
#endif
        return res_t::error;
      }

      return res_t::ok;
    }

    template <class T>
    typename std::enable_if<std::is_base_of<Serializable, typename clear_type<T>::type>::value, res_t>::type
    serialize_field_vector_serializable(const std::string& key, std::vector<T>* val, const std::string& logger_id, const std::string& doc_string,
                                        const bool& scheme, YAML::Emitter& node)
    {
      node << YAML::Key << key;
      if (scheme)
      {
        node << YAML::VerbatimTag("vector:serializable");
      }

      node << YAML::Value << YAML::Block << YAML::BeginSeq;
      for (auto& el : *val)
      {
        node << YAML::Value << YAML::BeginMap;
        call_as_ptr<T>::ptr(el)->set_logger(logger_id);
        if (call_as_ptr<T>::ptr(el)->leaf_serialize(node, scheme) != res_t::ok)
        {
#if ENABLE_SERDES_LOGGING
          CLOG(ERROR, logger_id.c_str()) << "failed to serialize key "
                                         << "\"" << key << "\".";
#endif
          return res_t::error;
        }
        node << YAML::EndMap;
      }
      node << YAML::EndSeq;

      if (scheme && doc_string.size())
      {
        node << YAML::Key << (key + "_doc") << YAML::VerbatimTag("doc");
        node << YAML::Value << doc_string;
      }

      if (!node.good())
      {
#if ENABLE_SERDES_LOGGING
        CLOG(ERROR, logger_id.c_str()) << "failed to serialize key "
                                       << "\"" << key << "\"."
                                       << " reason: " << node.GetLastError();
#endif
        return res_t::error;
      }
      return res_t::ok;
    }

    template <class T>
    typename std::enable_if<(!std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    serialize_field_vector_serializable(const std::string& key, std::vector<T>* val, const std::string& logger_id, const std::string& doc_string,
                                        const bool& scheme, YAML::Emitter& node)
    {
      return serialize_field_other(key, val, logger_id, doc_string, scheme, node);
    }

    template <class K, class T>
    typename std::enable_if<(!std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    serialize_field_map_serializable(const std::string& key, std::map<K, T>* val, const std::string& logger_id, const std::string& doc_string,
                                     const bool& scheme, YAML::Emitter& node)
    {
      return serialize_field_other(key, val, logger_id, doc_string, scheme, node);
    }

    template <class K, class T>
    typename std::enable_if<(std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    serialize_field_map_serializable(const std::string& key, std::map<K, T>* val, const std::string& logger_id, const std::string& doc_string,
                                     const bool& scheme, YAML::Emitter& node)
    {
      node << YAML::Key << key;
      if (scheme)
      {
        node << YAML::VerbatimTag(std::string("map:") + type_name<K>() + std::string(":serializable"));
      }
      node << YAML::Value << YAML::BeginMap;
      for (auto& el : *val)
      {
        node << YAML::Key << el.first;
        node << YAML::Value << YAML::BeginMap;
        el.second->set_logger(logger_id);
        if (el.second->leaf_serialize(node, scheme) != res_t::ok)
        {
#if ENABLE_SERDES_LOGGING
          CLOG(ERROR, logger_id.c_str()) << "failed to serialize key "
                                         << "\"" << key << "\".";
#endif
          return res_t::error;
        }
        node << YAML::EndMap;
      }
      node << YAML::EndMap;

      if (scheme && doc_string.size())
      {
        node << YAML::Key << (key + "_doc") << YAML::VerbatimTag("doc");
        node << YAML::Value << doc_string;
      }

      if (!node.good())
      {
#if ENABLE_SERDES_LOGGING
        CLOG(ERROR, logger_id.c_str()) << "failed to serialize key"
                                       << "\"" << key << "\"."
                                       << " reason: " << node.GetLastError();
#endif
        return res_t::error;
      }

      return res_t::ok;
    }

    template <class T>
    typename std::enable_if<is_vector<T>::value, res_t>::type serialize_field(const std::string& key, T* val, const std::string& logger_id,
                                                                              const std::string& doc_string, const bool& scheme, YAML::Emitter& node)
    {
      return serialize_field_vector_serializable(key, val, logger_id, doc_string, scheme, node);
    }

    template <class T>
    typename std::enable_if<is_map<T>::value, res_t>::type serialize_field(const std::string& key, T* val, const std::string& logger_id,
                                                                           std::string doc_string, const bool& scheme, YAML::Emitter& node)
    {
      return serialize_field_map_serializable(key, val, logger_id, doc_string, scheme, node);
    }

    template <class T>
    typename std::enable_if<(!is_stl_container<T>::value), res_t>::type serialize_field(const std::string& key, T* val, const std::string& logger_id,
                                                                                        const std::string& doc_string, const bool& scheme,
                                                                                        YAML::Emitter& node)
    {
      return serialize_field_other(key, val, logger_id, doc_string, scheme, node, (clear_type_v<decltype(val)>*)nullptr);
    }

    //! =============================deserialization===============================

    template <class T>
    res_t deserialize_field_other(const std::string& key, T* val, const std::string& logger_id, const YAML::Node& node, ...)
    {
      if (!node[key])
      {
#if ENABLE_SERDES_LOGGING
        CLOG(WARNING, logger_id.c_str()) << "no key "
                                         << "\"" << key << "\".";
#endif
        return res_t::key_not_found;
      }
      try
      {
        *val = node[key].as<T>();
        return res_t::ok;
      }
      catch (YAML::Exception e)
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
    res_t deserialize_field_other(const std::string& key, T* val, const std::string& logger_id, const YAML::Node& node, Serializable*)
    {
      if (!node[key])
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
    deserialize_field_vector_serializable(const std::string& key, std::vector<T>* val, const std::string& logger_id, const YAML::Node& node)
    {
      if (!node[key])
      {
#if ENABLE_SERDES_LOGGING
        CLOG(WARNING, logger_id.c_str()) << "no key "
                                         << "\"" << key << "\".";
#endif
        return res_t::key_not_found;
      }

      try
      {
        std::vector<YAML::Node> nodes = node[key].as<std::vector<YAML::Node>>();
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
      catch (YAML::Exception e)
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
    deserialize_field_vector_serializable(const std::string& key, std::vector<T>* val, const std::string& logger_id, const YAML::Node& node)
    {
      return deserialize_field_other(key, val, logger_id, node);
    }

    template <class K, class T>
    typename std::enable_if<(!std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    deserialize_field_map_serializable(const std::string& key, std::map<K, T>* val, const std::string& logger_id, const YAML::Node& node)
    {
      return deserialize_field_other(key, val, logger_id, node);
    }

    template <class K, class T>
    typename std::enable_if<(std::is_base_of<Serializable, typename clear_type<T>::type>::value), res_t>::type
    deserialize_field_map_serializable(const std::string& key, std::map<K, T>* val, const std::string& logger_id, const YAML::Node& node)
    {

      if (!node[key])
      {
#if ENABLE_SERDES_LOGGING
        CLOG(WARNING, logger_id.c_str()) << "no key "
                                         << "\"" << key << "\".";
#endif
        return res_t::key_not_found;
      }
      try
      {
        YAML::Node childs = node[key].as<YAML::Node>();

        stat_t stat;

        for (auto ch : childs)
        {
          std::string key  = ch.first.as<std::string>();
          YAML::Node  nval = ch.second.as<YAML::Node>();

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
        }

        auto totall_cnt = childs.size();
        if (stat.all_skiped_cnt == totall_cnt || stat.key_not_found_cnt == totall_cnt)
          return res_t::all_skiped;
        else if (stat.all_skiped_cnt > 0 || stat.key_not_found_cnt > 0 || stat.not_all_deser_cnt)
          return res_t::not_all_deser;

        return res_t::ok;
      }
      catch (YAML::Exception e)
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
                                                                             const YAML::Node& node)
    {
      return deserialize_field_map_serializable(key, val, logger_id, node);
    }
    template <class T>
    typename std::enable_if<is_vector<T>::value, res_t>::type deserialize_field(const std::string& key, T* val, const std::string& logger_id,
                                                                                const YAML::Node& node)
    {
      return deserialize_field_vector_serializable(key, val, logger_id, node);
    }

    template <class T>
    typename std::enable_if<(!is_stl_container<T>::value), res_t>::type deserialize_field(const std::string& key, T* val,
                                                                                          const std::string& logger_id, const YAML::Node& node)
    {
      return deserialize_field_other(key, val, logger_id, node, (clear_type_v<decltype(val)>*)nullptr);
    }
  } // namespace serdes
} // namespace proplib
