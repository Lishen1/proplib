#pragma once

#include <functional>
#include <map>
#include <string>

#include "config.h"
#include "tools.h"
#include "types.h"

//#include <easylogging++.h>

namespace proplib
{
  namespace serdes
  {
    template <typename T>
    void begin_map(T& node)
    {
    }

    template <typename T>
    void end_map(T& node)
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
    using VoidFunction = std::function<void()>;

  protected:
    struct Serializer_pair
    {
      Function serializer;
      Function deserializer;
    };

    char add_serdes_lambda(std::string _key, Function serializer, Function deserializer)
    {
      auto& lv_Pair        = _serializers[_key];
      lv_Pair.serializer   = serializer;
      lv_Pair.deserializer = deserializer;
      return 0;
    }

    char serialize_subs(VoidFunction function)
    {
      _subs_deser_func = function;
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

      if (_subs_deser_func)
        _subs_deser_func();

      Container<T> container(cont);
      stat_t       stat;

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

    // should be a protected friend function but too complicated to implement
  public:
    template <class T>
    res_t leaf_serialize(T& cont, const bool& scheme = false) const
    {
      _scheme = scheme;
      if (_subs_deser_func)
        _subs_deser_func();

      Container<clear_type_v<T>> container(cont);
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

  protected:
    std::string                          _logger_id;
    std::map<std::string, Serializable*> subprops;
    VoidFunction                         _subs_deser_func = nullptr;
    mutable bool                         _scheme          = false;

  private:
    std::map<std::string, Serializer_pair> _serializers;
  };
} // namespace proplib
