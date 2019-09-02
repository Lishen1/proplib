#pragma once

#include <QtWidgets/QWidget>
#include <yaml-cpp/yaml.h>
#include <functional>

namespace Ui
{
  class proplibqtguiClass;
}

enum class ui_build_res
{
  success = 0,
  type_cast_error,
  not_my_type,
  mixed_error,
};

class Iui_tree_elem;
class Ui_tree_root;
class QtnPropertyWidget;
class QtnPropertySet;
class proplibqtgui : public QWidget
{
    Q_OBJECT

public:
    proplibqtgui(QWidget *parent = Q_NULLPTR);
    ui_build_res build_gui(YAML::Node& n);
    ui_build_res update_gui(YAML::Node& n);
    void set_some_prop_changed_callback(std::function<void(Iui_tree_elem*)> f);
private:
    Ui::proplibqtguiClass* ui;
    YAML::Node _yaml_node;
    std::shared_ptr<Ui_tree_root> _root;
    std::shared_ptr<QtnPropertyWidget> _prop_widget;
    QtnPropertySet* _prop_set = nullptr;
    std::function<void(Iui_tree_elem*)> _some_prop_changed_callback;

protected:

};
