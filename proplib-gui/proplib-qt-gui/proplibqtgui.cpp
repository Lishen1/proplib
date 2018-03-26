#include "proplibqtgui.h"


#include <QtnProperty/Core/PropertyCore.h>
#include <QtnProperty/PropertyWidget/PropertyWidget.h>
#include <QtnProperty/Core/QObjectPropertySet.h>

#include <QHBoxLayout>
#include <QString>

#include <yaml-cpp/yaml.h>

#include <iostream>

bool load_property(const YAML::Node& node, QtnPropertySet* prop_set)
{
  for (auto ch: node)
  {

    QString key = QObject::tr(ch.first.as<std::string>().c_str());
    YAML::Node nval = ch.second.as<YAML::Node>();

    QString tag = nval.Tag().c_str();

    if (tag == "float")
    {
      auto v = nval.as<float>();
      auto val = new QtnPropertyFloat(prop_set);
      val->setName(key);
      val->setDescription(QObject::tr("Float value"));
      val->setValue(v);
    }
    else if (tag == "double")
    {
      auto v = nval.as<float>();
      auto val = new QtnPropertyDouble(prop_set);
      val->setName(key);
      val->setDescription(QObject::tr("Double value"));
      val->setValue(v);
    }
    else if (tag == "int")
    {
      auto v = nval.as<int>();
      auto val = new QtnPropertyInt(prop_set);
      val->setName(key);
      val->setDescription(QObject::tr("Int value"));
      val->setValue(v);
    }
    else if (tag == "string")
    {
      auto v = QString(nval.as<std::string>().c_str());
      auto val = new QtnPropertyQString(prop_set);
      val->setName(key);
      val->setDescription(QObject::tr("String value"));
      val->setValue(v);
    }
    else if (tag == "bool")
    {
      auto v = nval.as<bool>();
      auto val = new QtnPropertyBool(prop_set);
      val->setName(key);
      val->setDescription(QObject::tr("Bool value"));
      val->setValue(v);
    }
    else if (tag == "serializable")
    {

      QtnPropertySet* n_prop_set = new QtnPropertySet(prop_set->parent());
      n_prop_set->setName(key);
      load_property(nval, n_prop_set);
      prop_set->addChildProperty(n_prop_set);
    }
    else
    {
      QStringList sep_tags = tag.split(":");
      if (sep_tags.size() > 1)
      {
        if (sep_tags[0] == "vector")
        {
          QtnPropertySet* n_prop_set = new QtnPropertySet(prop_set->parent());
          n_prop_set->setName(key);
          prop_set->addChildProperty(n_prop_set);

          QString v_type = sep_tags[1];
          if (v_type == "float")
          {
            auto vec = nval.as<std::vector<float> >();
            uint32_t idx = 0;
            for (auto v: vec)
            {
              auto val = new QtnPropertyFloat(n_prop_set);
              val->setName(QString("[%1]").arg(idx));
              val->setDescription(QObject::tr("Float value"));
              val->setValue(v);
              idx++;
            }
          }
          else if (v_type == "double")
          {
            auto vec = nval.as<std::vector<double> >();
            uint32_t idx = 0;
            for (auto v: vec)
            {
              auto val = new QtnPropertyDouble(n_prop_set);
              val->setName(QString("[%1]").arg(idx));
              val->setDescription(QObject::tr("Float value"));
              val->setValue(v);
              idx++;
            }
          }
          else if (v_type == "int")
          {
            auto vec = nval.as<std::vector<int> >();
            uint32_t idx = 0;
            for (auto v: vec)
            {
              auto val = new QtnPropertyInt(n_prop_set);
              val->setName(QString("[%1]").arg(idx));
              val->setDescription(QObject::tr("Float value"));
              val->setValue(v);
              idx++;
            }
          }
          else if (v_type == "string")
          {
            auto vec = nval.as<std::vector<std::string> >();
            uint32_t idx = 0;
            for (auto v: vec)
            {
              auto val = new QtnPropertyQString(n_prop_set);
              val->setName(QString("[%1]").arg(idx));
              val->setDescription(QObject::tr("Float value"));
              val->setValue(QString(v.c_str()));
              idx++;
            }
          }
          else if (v_type == "serializable")
          {
            uint32_t idx = 0;
            for (auto ser : nval)
            {
              QtnPropertySet* ps = new QtnPropertySet(prop_set->parent());
              ps->setName(QString("[%1]").arg(idx));
              load_property(ser, ps);
              n_prop_set->addChildProperty(ps);
              idx++;
            }
          }
        }
      }
      else
        std::cout << "unknown tag " << tag.toLatin1().constData() << std::endl;
    }
  }

  return true;
}

proplibqtgui::proplibqtgui(QWidget *parent)
    : QWidget(parent)
{

  //QtnPropertyEnum* EnumProperty = new QtnPropertyEnum(nullptr);
    ui.setupUi(this);

    this->setLayout(new QHBoxLayout);

    QtnPropertyWidget *centralWidget = new QtnPropertyWidget();

    auto m_propertySet = new QtnPropertySet(centralWidget);

    this->layout()->addWidget(centralWidget);

    YAML::Node saved_yml = YAML::LoadFile(R"(D:\User_data\VC_PROJECTS\proplib\tests\prop-serialize\test.yml)");

    load_property(saved_yml, m_propertySet);


    //auto floatValue = new QtnPropertyFloat(m_propertySet);
    //floatValue->setName(tr("Value"));
    //floatValue->setDescription(tr("Float value"));
    //floatValue->setMaxValue(1.f);
    //floatValue->setMinValue(0.f);
    //floatValue->setStepValue(0.1f);
    //floatValue->setValue(0.3f);



    //auto m_propertySet2 = new QtnPropertySet(centralWidget);
    //m_propertySet2->setName("ololo");
    //m_propertySet->addChildProperty(m_propertySet2);

    centralWidget->setPropertySet(m_propertySet);
}
