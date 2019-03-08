#include "proplibqtgui.h"

#include <QtnProperty/Core/PropertyCore.h>
#include <QtnProperty/Core/QObjectPropertySet.h>
#include <QtnProperty/PropertyWidget/PropertyWidget.h>

#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QString>
#include <yaml-cpp/yaml.h>

#include <QDebug>

#include <iostream>

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
      prnt->add_child(this);
  }
  QtnPropertySet*      get_prop_set() { return _prop_set; }
  virtual ui_build_res set_node(YAML::Node& n)
  {
    _node = n;
    return set_val();
  }

  virtual ui_build_res set_node(YAML::detail::iterator_value it)
  {
    _node_iter.first = it.first;
    _node_iter.second = it.second;
    return set_val();
  }

  void                       add_child(Iui_tree_elem* child) { _childs.push_back(child); }
  std::list<Iui_tree_elem*>& get_childs() { return _childs; };

  protected:
  virtual ui_build_res set_val() = 0;

  protected:
  YAML::Node                   _node;
  YAML::detail::iterator_value _node_iter;
  Iui_tree_elem*               _parent   = nullptr;
  QtnPropertySet*              _prop_set = nullptr;
  std::list<Iui_tree_elem*>    _childs;
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
    _prop->setName(name);
    _prop->setDescription(doc_string);
    QObject::connect(_prop, &QtnProperty::propertyDidChange, this, &Ui_tree_elem<T>::on_property_did_change);
  }

  private:
  virtual void on_property_did_change(const QtnPropertyBase* changedProperty, const QtnPropertyBase* firedProperty, QtnPropertyChangeReason reason)
      = 0;

  protected:
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
      _node.as<YAML::Node>() = ((Ui_tree_elem<T>::Prop_type*)changedProperty)->value();
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
      auto v = _node.as<V>();
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
      _node.as<YAML::Node>() = std::string(((QtnPropertyQString*)changedProperty)->value().toLatin1().constData());
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
      _node.as<YAML::Node>() = ((QtnPropertyBool*)changedProperty)->value();
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
        (*child)->set_node(nod.as<YAML::Node>());
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
  typedef std::conditional<std::is_same<T, float>::value, QtnPropertyFloat, void>::type A;
  typedef std::conditional<std::is_same<T, int>::value, QtnPropertyInt, A>::type        B;
  typedef std::conditional<std::is_same<T, double>::value, QtnPropertyDouble, B>::type  C;

  try
  {

    auto elem = new Ui_num_tree_elem<C, T>(name, doc_string, prnt);
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
  if (typeid(T).name() == type_name)
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
    res = select_n_create_numeric_property<int>(ch, name, prnt, tag, doc_string);

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
      if(tag == "serializable")
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
  ui.setupUi(this);
  this->setLayout(new QHBoxLayout);
  QtnPropertyWidget* centralWidget = new QtnPropertyWidget();

  auto m_propertySet = new QtnPropertySet(centralWidget);

  this->layout()->addWidget(centralWidget);
  _root = new Ui_tree_root(nullptr, m_propertySet);
  centralWidget->setPropertySet(m_propertySet);
  startTimer(5000);
}

ui_build_res proplibqtgui::build_gui(YAML::Node& n)
{
  _yaml_node = n;
  return build_property_tree(n, _root);
}

ui_build_res proplibqtgui::update_gui(YAML::Node& n)
{
  _yaml_node = n;
  return _root->set_node(n);
}

void proplibqtgui::timerEvent(QTimerEvent* event)
{
  std::cout << "yaml\n" << _yaml_node << std::endl;
  std::cout << "timer event\n";
}
