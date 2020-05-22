#include <gui/proplibqtgui.h>
#include "ui_proplibqtgui.h"

#include <serialize/tools.h>

#include <PropertyCore.h>
#include <QObjectPropertySet.h>
#include <PropertyWidget.h>

#include <iostream>

#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QString>
#include <yaml-cpp/yaml.h>

#include <QDebug>


class Iui_tree_elem;

ui_build_res create_simple_prop(YAML::Node& ch, const QString& name, Iui_tree_elem* prnt, const QString& tag, const QString& doc_string);
ui_build_res build_property_tree(YAML::Node& node, Iui_tree_elem* prnt);

namespace
{
  bool is_doc_node(const YAML::Node& doc_node)
  {
    if (doc_node && doc_node.Tag() == "doc")
      return true;
    else
      return false;
  }
} // namespace

class Iui_tree_elem
{
  public:
  Iui_tree_elem(Iui_tree_elem* prnt, QtnPropertySet* prop_set) : _prop_set(prop_set)
  {
    if (prnt)
    {
      _parent = prnt;
      _parent->add_child(this);
    }
  }

  virtual ~Iui_tree_elem()
  {
    for (Iui_tree_elem* c : _childs)
    {
      delete c;
    }
    _childs.clear();
  }
  QtnPropertySet*      get_prop_set() { return _prop_set; }
  virtual ui_build_res set_node(YAML::Node& n)
  {
    _node = n;
    return set_val();
  }

  virtual ui_build_res set_node(YAML::detail::iterator_value it)
  {
    _node_iter.first  = it.first;
    _node_iter.second = it.second;
    return set_val();
  }

  virtual void some_prop_changed(Iui_tree_elem* p)
  {
    if (_parent)
    {
      _parent->some_prop_changed(p);
    }

    if (_some_prop_changed_callback)
      _some_prop_changed_callback(p);
  }

  void set_some_prop_changed_callback(std::function<void(Iui_tree_elem*)> f) { _some_prop_changed_callback = f; }

  void                       add_child(Iui_tree_elem* child) { _childs.push_back(child); }
  std::list<Iui_tree_elem*>& get_childs() { return _childs; };

  protected:
  virtual ui_build_res set_val() = 0;

  public:
  YAML::Node                          _node;
  YAML::detail::iterator_value        _node_iter;
  Iui_tree_elem*                      _parent   = nullptr;
  QtnPropertySet*                     _prop_set = nullptr;
  std::list<Iui_tree_elem*>           _childs;
  std::function<void(Iui_tree_elem*)> _some_prop_changed_callback;
};

class Ui_tree_root : public Iui_tree_elem
{
  public:
  Ui_tree_root(Iui_tree_elem* prnt, QtnPropertySet* prop_set) : Iui_tree_elem(prnt, prop_set) {}

  protected:
  virtual ui_build_res set_val() override
  {

    int  idx   = 0;
    auto child = _childs.begin();
    for (YAML::detail::iterator_value nod : _node)
    {
      if (is_doc_node(nod.second))
        continue;

      if (child != _childs.end())
      {
        (*child)->set_node(nod.second);
        child++;
      }
    }
    return ui_build_res::success;
  }
};

class Ui_pair_elem : public Iui_tree_elem
{
  public:
  Ui_pair_elem(Iui_tree_elem* prnt, QtnPropertySet* prop_set) : Iui_tree_elem(prnt, prop_set) {}

  protected:
  virtual ui_build_res set_val() override
  {
    if (_childs.size() != 2)
      return ui_build_res::not_my_type;

    auto child = _childs.begin();

    (*child)->set_node(_node_iter.first);
    child++;
    (*child)->set_node(_node_iter.second);

    return ui_build_res::success;
  }
};

template <typename T>
class Ui_tree_elem : public QObject, public Iui_tree_elem
{

  public:
  typedef T Prop_type;
  Ui_tree_elem(const QString& name, const QString& doc_string, Iui_tree_elem* prnt) : Iui_tree_elem(prnt, prnt->get_prop_set())
  {
    _prop = new T(_prop_set);
    _prop_set->addChildProperty(_prop);

    _prop->setName(name);
    _prop->setDescription(doc_string);
    QObject::connect(_prop, &QtnProperty::propertyDidChange, this, &Ui_tree_elem<T>::on_change);
  }

  private:
  virtual void on_change(const QtnPropertyBase* changedProperty, const QtnPropertyBase* firedProperty, QtnPropertyChangeReason reason)
  {
    on_property_did_change(changedProperty, firedProperty, reason);
    if (reason & QtnPropertyChangeReasonNewValue || reason & QtnPropertyChangeReasonLoadedValue)
      some_prop_changed(this);
  }

  virtual void on_property_did_change(const QtnPropertyBase* changedProperty, const QtnPropertyBase* firedProperty, QtnPropertyChangeReason reason)
      = 0;

  public:
  T* _prop = nullptr;
};

template <typename T, typename V>
class Ui_num_tree_elem : public Ui_tree_elem<T>
{

  public:
  Ui_num_tree_elem(const QString& name, const QString& doc_string, Iui_tree_elem* prnt) : Ui_tree_elem<T>(name, doc_string, prnt) {}

  private:
  virtual void on_property_did_change(const QtnPropertyBase* changedProperty, const QtnPropertyBase* firedProperty, QtnPropertyChangeReason reason)
  {
    try
    {
      std::string tag        = Iui_tree_elem::_node.Tag();
      auto prop_type = (T*)(changedProperty);
      Iui_tree_elem::_node.as<YAML::Node>() = prop_type->value();
      Iui_tree_elem::_node.SetTag(tag);
    }
    catch (std::exception& e)
    {
      qCritical() << e.what();
    }
  }
  virtual ui_build_res set_val()
  {
    try
    {
      auto v = Iui_tree_elem::_node.as<V>();
      this->_prop->setValue(v);
    }
    catch (std::exception& e)
    {
      qCritical() << e.what();
      return ui_build_res::type_cast_error;
    }
    return ui_build_res::success;
  }
};

class Ui_string_tree_elem : public Ui_tree_elem<QtnPropertyQString>
{
  public:
  Ui_string_tree_elem(const QString& name, const QString& doc_string, Iui_tree_elem* prnt) : Ui_tree_elem<QtnPropertyQString>(name, doc_string, prnt)
  {
  }

  private:
  virtual void on_property_did_change(const QtnPropertyBase* changedProperty, const QtnPropertyBase* firedProperty, QtnPropertyChangeReason reason)
  {
    try
    {
      std::string tag        = _node.Tag();
      _node.as<YAML::Node>() = std::string(((QtnPropertyQString*)changedProperty)->value().toLatin1().constData());
      _node.SetTag(tag);
    }
    catch (std::exception& e)
    {
      qCritical() << e.what();
    }
  }
  virtual ui_build_res set_val()
  {

    try
    {
      auto v = QString(_node.as<std::string>().c_str());
      _prop->setValue(v);
    }
    catch (std::exception& e)
    {
      qCritical() << e.what();
      return ui_build_res::type_cast_error;
    }
    return ui_build_res::success;
  }
};

class Ui_bool_tree_elem : public Ui_tree_elem<QtnPropertyBool>
{
  public:
  Ui_bool_tree_elem(const QString& name, const QString& doc_string, Iui_tree_elem* prnt) : Ui_tree_elem<QtnPropertyBool>(name, doc_string, prnt) {}

  private:
  virtual void on_property_did_change(const QtnPropertyBase* changedProperty, const QtnPropertyBase* firedProperty, QtnPropertyChangeReason reason)
  {
    try
    {
      std::string tag        = _node.Tag();
      _node.as<YAML::Node>() = ((QtnPropertyBool*)changedProperty)->value();
      _node.SetTag(tag);
    }
    catch (std::exception& e)
    {
      qCritical() << e.what();
    }
  }
  virtual ui_build_res set_val()
  {
    try
    {
      auto v = _node.as<bool>();
      _prop->setValue(v);
    }
    catch (std::exception& e)
    {
      qCritical() << e.what();
      return ui_build_res::type_cast_error;
    }
    return ui_build_res::success;
  }
};

class Ui_vec_tree_elem : public Iui_tree_elem
{

  public:
  Ui_vec_tree_elem(const QString& name, const QString& doc_string, const QString& v_type, Iui_tree_elem* prnt)
  : Iui_tree_elem(prnt, new QtnPropertySet(prnt->get_prop_set())), _v_type(v_type)
  {
    prnt->get_prop_set()->addChildProperty(_prop_set);
    _prop_set->setName(name);
    _prop_set->setDescription(doc_string);
  }

  virtual ui_build_res set_val() override
  {
    if (!_node.IsSequence())
      return ui_build_res::type_cast_error;

    int  idx   = 0;
    auto child = _childs.begin();
    for (YAML::detail::iterator_value nod : _node)
    {
      if (is_doc_node(nod))
        continue;
      if (child != _childs.end())
      {
        YAML::Node node = nod.as<YAML::Node>();
        (*child)->set_node(node);
        child++;
      }
    }
    return ui_build_res::success;
  }

  protected:
  QString _v_type;
};

class Ui_map_tree_elem : public Iui_tree_elem
{

  public:
  Ui_map_tree_elem(const QString& name, const QString& doc_string, const QString& k_type, const QString& v_type, Iui_tree_elem* prnt)
  : Iui_tree_elem(prnt, new QtnPropertySet(prnt->get_prop_set())), _k_type(k_type), _v_type(v_type)
  {
    prnt->get_prop_set()->addChildProperty(_prop_set);
    _prop_set->setName(name);
    _prop_set->setDescription(doc_string);
  }

  virtual ui_build_res set_val() override
  {

    if (!_node.IsMap())
      return ui_build_res::type_cast_error;

    int  idx   = 0;
    auto child = _childs.begin();
    for (YAML::detail::iterator_value nod : _node)
    {
      if (is_doc_node(nod))
        continue;

      if (child != _childs.end())
      {
        (*child)->set_node(nod);
        child++;
      }
    }
    return ui_build_res::success;
  }

  protected:
  QString _k_type;
  QString _v_type;
};

class Ui_serial_tree_elem : public Iui_tree_elem
{

  public:
  Ui_serial_tree_elem(const QString& name, const QString& doc_string, Iui_tree_elem* prnt, YAML::Node& node)
  : Iui_tree_elem(prnt, new QtnPropertySet(prnt->get_prop_set()))
  {
    prnt->get_prop_set()->addChildProperty(_prop_set);
    _prop_set->setName(name);
    _prop_set->setDescription(doc_string);
  }

  virtual ui_build_res set_val() override
  {
    int  idx   = 0;
    auto child = _childs.begin();
    for (YAML::detail::iterator_value nod : _node)
    {
      if (is_doc_node(nod.second))
        continue;
      std::cout << nod.second << std::endl;
      if (child != _childs.end())
      {
        (*child)->set_node(nod.second);
        child++;
      }
    }
    return ui_build_res::success;
  }

  protected:
};

template <typename T>
ui_build_res create_numeric_property(YAML::Node& ch, const QString& name, Iui_tree_elem* prnt, const QString& doc_string)
{
  typedef std::conditional_t<std::is_same_v<T, float>, QtnPropertyFloat, void> A;
  typedef std::conditional_t<std::is_same_v<T, double>, QtnPropertyDouble, A>  B;
  typedef std::conditional_t<std::is_same_v<T, uint8_t>, QtnPropertyUInt, B>   C;
  typedef std::conditional_t<std::is_same_v<T, uint16_t>, QtnPropertyUInt, C>  D;
  typedef std::conditional_t<std::is_same_v<T, uint32_t>, QtnPropertyUInt, D>  E;
  typedef std::conditional_t<std::is_same_v<T, uint64_t>, QtnPropertyUInt, E>  F;
  typedef std::conditional_t<std::is_same_v<T, int8_t>, QtnPropertyInt, F>     G;
  typedef std::conditional_t<std::is_same_v<T, int16_t>, QtnPropertyInt, G>    J;
  typedef std::conditional_t<std::is_same_v<T, int32_t>, QtnPropertyInt, J>    K;
  typedef std::conditional_t<std::is_same_v<T, int64_t>, QtnPropertyInt, K>    M;

  try
  {

    auto elem = new Ui_num_tree_elem<M, T>(name, doc_string, prnt);
    return elem->set_node(ch);
  }
  catch (const YAML::Exception& e)
  {
    qWarning() << "yaml exception: " << e.msg.c_str() << " in line " << e.mark.line << " in col " << e.mark.column;
    return ui_build_res::type_cast_error;
  }
}

template <typename T>
ui_build_res select_n_create_numeric_property(YAML::Node& ch, const QString& name, Iui_tree_elem* prnt, const QString& type_name,
                                              const QString& doc_string)
{
  if (std::string(proplib::type_name<T>()).c_str() == type_name)
    return create_numeric_property<T>(ch, name, prnt, doc_string);
  return ui_build_res::not_my_type;
}

ui_build_res create_string_property(YAML::Node& ch, const QString& name, Iui_tree_elem* prnt, const QString& doc_string)
{
  try
  {
    auto elem = new Ui_string_tree_elem(name, doc_string, prnt);
    return ui_build_res::success;
  }
  catch (const YAML::Exception& e)
  {
    qWarning() << "yaml exception: " << e.msg.c_str() << " in line " << e.mark.line << " in col " << e.mark.column;
    return ui_build_res::type_cast_error;
  }
}

ui_build_res create_bool_property(YAML::Node& ch, const QString& name, Iui_tree_elem* prnt, const QString& doc_string)
{
  try
  {

    auto elem = new Ui_bool_tree_elem(name, doc_string, prnt);
    return ui_build_res::success;
  }
  catch (const YAML::Exception& e)
  {
    qWarning() << "yaml exception: " << e.msg.c_str() << " in line " << e.mark.line << " in col " << e.mark.column;
    return ui_build_res::type_cast_error;
  }
}

ui_build_res create_simple_prop(YAML::Node& ch, const QString& name, Iui_tree_elem* prnt, const QString& tag, const QString& doc_string)
{

  ui_build_res res = ui_build_res::success;
  res              = select_n_create_numeric_property<float>(ch, name, prnt, tag, doc_string);

  if (res != ui_build_res::success && res != ui_build_res::type_cast_error)
    res = select_n_create_numeric_property<double>(ch, name, prnt, tag, doc_string);
  if (res != ui_build_res::success && res != ui_build_res::type_cast_error)
    res = select_n_create_numeric_property<float>(ch, name, prnt, tag, doc_string);
  if (res != ui_build_res::success && res != ui_build_res::type_cast_error)
    res = select_n_create_numeric_property<uint8_t>(ch, name, prnt, tag, doc_string);
  if (res != ui_build_res::success && res != ui_build_res::type_cast_error)
    res = select_n_create_numeric_property<uint16_t>(ch, name, prnt, tag, doc_string);
  if (res != ui_build_res::success && res != ui_build_res::type_cast_error)
    res = select_n_create_numeric_property<uint32_t>(ch, name, prnt, tag, doc_string);
  if (res != ui_build_res::success && res != ui_build_res::type_cast_error)
    res = select_n_create_numeric_property<uint64_t>(ch, name, prnt, tag, doc_string);
  if (res != ui_build_res::success && res != ui_build_res::type_cast_error)
    res = select_n_create_numeric_property<int8_t>(ch, name, prnt, tag, doc_string);
  if (res != ui_build_res::success && res != ui_build_res::type_cast_error)
    res = select_n_create_numeric_property<int16_t>(ch, name, prnt, tag, doc_string);
  if (res != ui_build_res::success && res != ui_build_res::type_cast_error)
    res = select_n_create_numeric_property<int32_t>(ch, name, prnt, tag, doc_string);
  if (res != ui_build_res::success && res != ui_build_res::type_cast_error)
    res = select_n_create_numeric_property<int64_t>(ch, name, prnt, tag, doc_string);

  if (res != ui_build_res::not_my_type)
    return res;

  if (tag == "string")
  {
    return create_string_property(ch, name, prnt, doc_string);
  }
  else if (tag == "bool")
  {
    return create_bool_property(ch, name, prnt, doc_string);
  }
  else if (tag == "serializable")
  {
    auto elem = new Ui_serial_tree_elem(name, doc_string, prnt, ch);
    return ui_build_res::success;
  }

  return ui_build_res::not_my_type;
}

ui_build_res build_property_tree(YAML::Node& node, Iui_tree_elem* prnt)
{
  ui_build_res res      = ui_build_res::success;
  YAML::Node   node_cpy = YAML::Clone(node);
  for (YAML::detail::iterator_value ch : node)
  {
    QString     key  = QObject::tr(ch.first.as<std::string>().c_str());
    YAML::Node& nval = ch.second;

    QString tag = nval.Tag().c_str();

    if (tag == "doc")
      continue;

    QString doc_string;

    YAML::Node doc_node = node_cpy[(key + "_doc").toLatin1().constData()];
    if (doc_node && doc_node.Tag() == "doc")
      doc_string = doc_node.as<std::string>().c_str();

    res = create_simple_prop(nval, key, prnt, tag, doc_string);
    if (res != ui_build_res::not_my_type)
    {
      if (tag == "serializable")
        build_property_tree(nval, prnt->get_childs().back());

      continue;
    }

    QStringList sep_tags = tag.split(":");
    if (sep_tags.size() > 1)
    {
      if (sep_tags[0] == "vector")
      {
        auto vec_prop = new Ui_vec_tree_elem(key, doc_string, sep_tags[1], prnt);

        int idx = 0;

        int type_cast_error_cnt = 0;
        int not_my_type_cnt     = 0;

        for (auto vv : nval)
        {
          ui_build_res lres = create_simple_prop(vv, QString("[%1]").arg(idx), vec_prop, sep_tags[1], "");
          ui_build_res rres = build_property_tree(vv, vec_prop->get_childs().back());

          if (lres == ui_build_res::type_cast_error)
            type_cast_error_cnt++;
          if (lres == ui_build_res::not_my_type)
            not_my_type_cnt++;

          if (rres == ui_build_res::type_cast_error)
            type_cast_error_cnt++;
          if (rres == ui_build_res::not_my_type)
            not_my_type_cnt++;

          idx++;
        }

        if (not_my_type_cnt && type_cast_error_cnt)
          res = ui_build_res::mixed_error;
        else if (not_my_type_cnt)
          res = ui_build_res::not_my_type;
        else if (type_cast_error_cnt)
          res = ui_build_res::type_cast_error;
        else
          res = ui_build_res::success;
      }
      else if (sep_tags[0] == "map")
      {
        auto vec_prop = new Ui_map_tree_elem(key, doc_string, sep_tags[1], sep_tags[2], prnt);

        const QString& k_type              = sep_tags[1];
        const QString& v_type              = sep_tags[2];
        uint32_t       idx                 = 0;
        int            type_cast_error_cnt = 0;
        int            not_my_type_cnt     = 0;
        for (auto vv : nval)
        {

          QtnPropertySet* m_prop_set = new QtnPropertySet(vec_prop->get_prop_set());
          vec_prop->get_prop_set()->addChildProperty(m_prop_set);

          m_prop_set->setName(QString("[%1]").arg(idx));

          auto pr = new Ui_pair_elem(vec_prop, m_prop_set);

          ui_build_res kres = create_simple_prop(vv.first, "key", pr, k_type, "");
          ui_build_res vres = create_simple_prop(vv.second, "val", pr, v_type, "");

          if (kres == ui_build_res::type_cast_error)
            type_cast_error_cnt++;
          if (kres == ui_build_res::not_my_type)
            not_my_type_cnt++;

          if (vres == ui_build_res::type_cast_error)
            type_cast_error_cnt++;
          if (vres == ui_build_res::not_my_type)
            not_my_type_cnt++;

          idx++;
        }

        if (not_my_type_cnt && type_cast_error_cnt)
          res = ui_build_res::mixed_error;
        else if (not_my_type_cnt)
          res = ui_build_res::not_my_type;
        else if (type_cast_error_cnt)
          res = ui_build_res::type_cast_error;
        else
          res = ui_build_res::success;
      }
      else
        qWarning() << "unknown tag " << tag;
    }
    else
      qWarning() << "unknown tag " << tag;
  }

  return res;
}

void bypass_prop(QtnPropertySet* ps)
{
  if (ps)
    for (auto cs : ps->childProperties())
    {
      qDebug() << "cs name " << cs->name();
      bypass_prop(cs->asPropertySet());
    }
}

proplibqtgui::proplibqtgui(QWidget* parent) : QWidget(parent)
{
  ui = new Ui::proplibqtguiClass;
  ui->setupUi(this);
  this->setLayout(new QHBoxLayout);

  _prop_widget = std::make_shared<QtnPropertyWidget>();
  layout()->addWidget(_prop_widget.get());

  _prop_set = new QtnPropertySet(_prop_widget.get());
  _root = std::make_shared<Ui_tree_root>(nullptr, _prop_set);
  _prop_widget->setPropertySet(_prop_set);
}

ui_build_res proplibqtgui::build_gui(YAML::Node& n)
{

  _yaml_node       = n;
  ui_build_res res = build_property_tree(n, _root.get());
  //this->adjustSize();

  // set callback after loading to not trig it so usual
  _root->set_some_prop_changed_callback(_some_prop_changed_callback);

  return res;
}

ui_build_res proplibqtgui::update_gui(YAML::Node& n)
{
  _yaml_node = n;
  return _root->set_node(n);
}

void proplibqtgui::set_some_prop_changed_callback(std::function<void(Iui_tree_elem*)> f)
{
  _some_prop_changed_callback = f;
}
