#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include "ui_proplibqteditor.h"
#include <yaml-cpp/yaml.h>
#include <mutex>
#include "QMap"

class proplibqtgui;

class proplibqteditor : public QMainWindow
{
    Q_OBJECT

public:
    proplibqteditor(QWidget *parent = Q_NULLPTR);
    void open_config(const QString& path);
public slots:
  void open_config_diag(const bool& on);
  void save_config_diag(const bool& on);
  void cursor_position_changed();
  void config_changed();
private:
    Ui::proplibqteditorClass ui;
    proplibqtgui* _prop_gui;
    QTextEdit* _yaml_viewer;
    QTextCursor _yaml_viewer_cursor;
    YAML::Node _current_config;
    std::mutex _server_mutex;

    QMap<QString, QByteArray> _server_request;
    QMap<QString, uint32_t> _has_request;

protected:
  virtual void timerEvent(QTimerEvent *event) override;

};
