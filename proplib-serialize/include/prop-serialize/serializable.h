#pragma once
/*!
 * \file serializable.h
 *
 * \author Пантелюк П.А
 * \date Март 2018
 *
 *
 */

#include "config.h"
#include "tools.h"
#include "types.h"
#include <easylogging++.h>
#include <functional>
#include <map>
#include <string>

namespace proplib
{
  class IContainer
  {
    public:
    IContainer() {}
    virtual ~IContainer(){};
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
    protected:
    typedef std::function<res_t(const std::string&, IContainer*)> Func;
    typedef std::function<void()> NRFunc;

    struct Serializer_pair
    {
      Func serializer;
      Func deserializer;
    };

    Serializable()              = default;
    Serializable(Serializable&) = default;
    Serializable& operator=(Serializable&) = default;
    ~Serializable()                        = default;

    char add_serdes_lambda(std::string _key, Func serializer, Func deserializer)
    {
      auto& lv_Pair        = _serializers[_key];
      lv_Pair.serializer   = serializer;
      lv_Pair.deserializer = deserializer;
      return 0;
    }

    char serialize_subs(NRFunc func)
    {
      _subs_deser_func = func;
      return 0;
    }

   

    public:
      bool add_subprop(Serializable* p, const std::string& name) { subprops[name] = p; return true; }
    void set_logger(const std::string& logger_id) { _logger_id = logger_id; }
    template <class T>
    res_t serialize(T& cont) const
    {
      if (_subs_deser_func)
        _subs_deser_func();

      Container<clear_type<T>::type> container(cont);
      for (auto& lv_ser : _serializers)
        if (lv_ser.second.serializer(lv_ser.first, &container) != res_t::ok)
        {
#if ENABLE_SERDES_LOGGING
          CLOG(ERROR, _logger_id.c_str()) << "failed to serialize key "
                                          << "\"" << lv_ser.first << "\""
                                          << " with container "
                                          << "\"" << typeid(T).name() << "\".";
#endif
          return res_t::error;
        }

      return res_t::ok;
    }

    template <class T>
    res_t deserialize(T& cont) const
    {

      if (_subs_deser_func)
        _subs_deser_func();

      Container<T> container(cont);
      stat_t stat;

      for (auto& lv_ser : _serializers)
      {
        auto res = lv_ser.second.deserializer(lv_ser.first, &container);
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

    protected:
    std::string _logger_id;
    std::map<std::string, Serializable*> subprops;
    NRFunc _subs_deser_func = nullptr;

    private:
    std::map<std::string, Serializer_pair> _serializers;

  };
} // namespace proplib
