#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "config.h"
#include "tools.h"
#include "types.h"

//#include <easylogging++.h>

namespace proplib
{
  namespace serdes
  {

    template <typename T>
    static void begin_map(T& node)
    {
    }

    template <typename T>
    static void end_map(T& node)
    {
    }

  } // namespace serdes

  class IContainer
  {
  public:
    virtual ~IContainer() = default;
  };

  template <class Ser>
  class Container : public IContainer
  {
  public:
    Container(Ser& node) : _node(node){};
    Ser&       node() { return _node; }
    const Ser& node() const { return _node; }

  private:
    Ser& _node;
  };

  class Serializable
  {
    using Function     = std::function<res_t(const std::string&, IContainer*)>;
    using Functions    = std::vector<Function>;
    using VoidFunction = std::function<void()>;

  protected:
    struct Serializer_pair
    {
      Functions serializer;
      Functions deserializer;
    };

    char add_serdes_lambda(std::string _key, Function serializer, Function deserializer)
    {
      auto& lv_Pair = _serializers[_key];
      lv_Pair.serializer.push_back(serializer);
      lv_Pair.deserializer.push_back(deserializer);
      return 0;
    }

    char serialize_subs(VoidFunction function)
    {
      _subs_deser_func.push_back(function);
      return 0;
    }

  public:
    bool add_subprop(Serializable* p, const std::string& name)
    {
      subprops[name] = p;
      return true;
    }

    void set_logger(const std::string& logger_id) { _logger_id = logger_id; }

    template <class T>
    res_t serialize(T& cont, const bool& scheme = false) const
    {
      serdes::begin_map(cont);
      res_t res = this->template leaf_serialize<T>(cont, scheme);
      serdes::end_map(cont);
      return res;
    }

    template <class T>
    res_t deserialize(T& cont) const
    {

      if (!_subs_deser_func.empty())
      {
        for (auto& func : _subs_deser_func)
        {
          func();
        }
      }

      Container<T> container(cont);
      stat_t       stat;

      for (auto& lv_ser : _serializers)
      {
        res_t res = res_t::error;
        for (auto& deser : lv_ser.second.deserializer)
        {
          res = deser(lv_ser.first, &container);
          if (res == res_t::ok)
            break;
        }

        // auto res = lv_ser.second.deserializer(lv_ser.first, &container);

        if (res == res_t::error)
          return res;

        if (res == res_t::all_skiped)
          stat.all_skiped_cnt++;
        else if (res == res_t::key_not_found)
          stat.key_not_found_cnt++;
        else if (res == res_t::not_all_deser)
          stat.not_all_deser_cnt++;
      }

      auto totall_cnt = _serializers.size();
      if (stat.all_skiped_cnt == totall_cnt || stat.key_not_found_cnt == totall_cnt)
        return res_t::all_skiped;
      else if (stat.all_skiped_cnt > 0 || stat.key_not_found_cnt > 0 || stat.not_all_deser_cnt)
        return res_t::not_all_deser;

      return res_t::ok;
    }

    // should be a protected friend function but too complicated to implement
  public:
    template <class T>
    res_t leaf_serialize(T& cont, const bool& scheme = false) const
    {
      _scheme = scheme;
      if (!_subs_deser_func.empty())
      {
        for (auto& func : _subs_deser_func)
        {
          func();
        }
      }

      Container<clear_type_v<T>> container(cont);
      for (auto& lv_ser : _serializers)
      {
        res_t val = res_t::error;
        for (auto& ser : lv_ser.second.serializer)
        {
          val = ser(lv_ser.first, &container);
          if (val == res_t::ok)
            break;
        }

        if (val != res_t::ok)
        {
#if ENABLE_SERDES_LOGGING
          CLOG(ERROR, _logger_id.c_str()) << "failed to serialize key "
                                          << "\"" << lv_ser.first << "\""
                                          << " with container "
                                          << "\"" << typeid(T).name() << "\".";
#endif
          return res_t::error;
        }
      }
      return res_t::ok;
    }

  protected:
    std::string                          _logger_id;
    std::map<std::string, Serializable*> subprops;
    std::vector<VoidFunction>            _subs_deser_func;
    // VoidFunction                         _subs_deser_func = nullptr;
    mutable bool _scheme = false;

  private:
    std::map<std::string, Serializer_pair> _serializers;
  };

  namespace serdes
  {
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
  } // namespace serdes
} // namespace proplib
