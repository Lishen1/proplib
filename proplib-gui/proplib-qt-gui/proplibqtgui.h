#pragma once

#include <QtWidgets/QWidget>
#include "ui_proplibqtgui.h"
#include <yaml-cpp/yaml.h>


enum class ui_build_res
{
  success = 0,
  type_cast_error,
  not_my_type,
  mixed_error,
};

class Ui_tree_root;
class proplibqtgui : public QWidget
{
    Q_OBJECT

public:
    proplibqtgui(QWidget *parent = Q_NULLPTR);
    ui_build_res build_gui(YAML::Node& n);
    ui_build_res update_gui(YAML::Node& n);
private:
    Ui::proplibqtguiClass ui;
    YAML::Node _yaml_node;
    Ui_tree_root* _root;
protected:
  virtual void timerEvent(QTimerEvent *event) override;

};
