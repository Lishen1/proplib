#include "proplibqteditor.h"
#include <proplibqtgui.h>
#include "QElapsedTimer"
#include <QFileDialog>
#include <iostream>
#include <strstream>
#include "QAbstractScrollArea"
#include "QScrollBar"
#include <QDebug>
#include "Yaml_highlighter.h"
#include <QFile>
#include "httplib.h"
struct Http_urls
{
  const static std::string build_gui_url;
  const static std::string update_gui_url;
  const static std::string update_struct_url;
};

const std::string Http_urls::build_gui_url = "/build_gui";
const std::string Http_urls::update_gui_url = "/update_gui";
const std::string Http_urls::update_struct_url = "/update_struct";

proplibqteditor::proplibqteditor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QPalette p;
    p.setColor(QPalette::Background, Qt::gray);
    //ui.splitter->setPalette(p);

    _prop_gui = new proplibqtgui;

    _prop_gui->set_some_prop_changed_callback([this](Iui_tree_elem* c)
    {
      config_changed();
    });

    ui.widget->layout()->addWidget(_prop_gui);

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    _yaml_viewer = new QTextEdit;

    Yaml_highlighter* highlighter = new Yaml_highlighter(_yaml_viewer->document());

    //_yaml_viewer->setFont(font);

    ui.widget_2->layout()->addWidget(_yaml_viewer);

    QObject::connect(ui.actionOpen_Config, &QAction::triggered, this, &proplibqteditor::open_config_diag);
    QObject::connect(ui.actionSave_Config, &QAction::triggered, this, &proplibqteditor::save_config_diag);
    QObject::connect(_yaml_viewer, &QTextEdit::cursorPositionChanged, this, &proplibqteditor::cursor_position_changed);


    std::thread* th = new std::thread([this]()
    {
      httplib::Server svr;

      svr.Post(Http_urls::build_gui_url.c_str(), [&](const httplib::Request& req, httplib::Response& res)
      {
        _server_mutex.lock();
        _server_request[Http_urls::build_gui_url.c_str()] = QByteArray(req.body.c_str(), req.body.size());
        _has_request[Http_urls::build_gui_url.c_str()] = 1;
        _server_mutex.unlock();
      });

      svr.Post(Http_urls::update_gui_url.c_str(), [&](const httplib::Request& req, httplib::Response& res)
      {
        _server_mutex.lock();
        _server_request[Http_urls::update_gui_url.c_str()] = QByteArray(req.body.c_str(), req.body.size());
        _has_request[Http_urls::update_gui_url.c_str()] = 1;
        _server_mutex.unlock();
      });

      svr.Get(Http_urls::update_struct_url.c_str(), [&](const httplib::Request& req, httplib::Response& res)
      {
        _server_mutex.lock();
        std::strstream ss;
        ss << _current_config << '\0';
        res.set_content(ss.str(), "text/plain");
        _server_mutex.unlock();
      });

      svr.listen("localhost", 1234);
    });
    
    startTimer(1);

    //open_config(R"(D:\User_data\VC_PROJECTS\proplib\tests\prop-serialize\test.yml)");
}


void proplibqteditor::open_config(const QString& path)
{
  _current_config = YAML::LoadFile(path.toLatin1().constData());
  QElapsedTimer timer;
  timer.start();
  _prop_gui->build_gui(_current_config);
  _prop_gui->update_gui(_current_config);
  qDebug() << QString("build_gui time ") << timer.elapsed();
  config_changed();
  this->adjustSize();
}

void proplibqteditor::open_config_diag(const bool& on)
{
  QString file_name = QFileDialog::getOpenFileName(this,
    tr("Open Config"), R"(D:\User_data\VC_PROJECTS\proplib\tests\prop-serialize)", tr("Config Files (*.yml *.yaml)"));

  if(!file_name.isEmpty())
    open_config(file_name);
}

void proplibqteditor::save_config_diag(const bool& on)
{
  QString file_name = QFileDialog::getSaveFileName(this,
    tr("Save Config"), R"(D:\User_data\VC_PROJECTS\proplib\tests\prop-serialize)", tr("Config Files (*.yml *.yaml)"));

  if (!file_name.isEmpty())
  {
    QFile file(file_name);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    std::strstream ss;
    ss << _current_config << '\0';
    QTextStream out(&file);
    out << ss.str();
    file.close();
  }
}

void proplibqteditor::cursor_position_changed()
{
  //_yaml_viewer_cursor = _yaml_viewer->textCursor();
}

void proplibqteditor::config_changed()
{
  std::strstream ss;
  ss << _current_config << '\0';

  std::string text = ss.str();

  const QTextCursor old_cursor = _yaml_viewer->textCursor();
  const int old_scrollbar_value = _yaml_viewer->verticalScrollBar()->value();

  _yaml_viewer->clear();
  _yaml_viewer->setPlainText(ss.str());
  _yaml_viewer->verticalScrollBar()->setValue(old_scrollbar_value);
}

void proplibqteditor::timerEvent(QTimerEvent *event)
{
  if (_has_request[Http_urls::build_gui_url.c_str()])
  {
    _server_mutex.lock();
    //qDebug() << _server_request[Http_urls::build_gui_url.c_str()];

    _current_config = YAML::Load(_server_request[Http_urls::build_gui_url.c_str()].constData());

    QElapsedTimer timer;
    timer.start();
    _prop_gui->build_gui(_current_config);
    _prop_gui->update_gui(_current_config);
    this->adjustSize();
    qDebug() << QString("build_gui time ") << timer.elapsed();
    config_changed();
    _server_mutex.unlock();
    _has_request[Http_urls::build_gui_url.c_str()] = 0;
  }

  if (_has_request[Http_urls::update_gui_url.c_str()])
  {
    _server_mutex.lock();

    _current_config = YAML::Load(_server_request[Http_urls::update_gui_url.c_str()].constData());

    QElapsedTimer timer;
    timer.start();
    _prop_gui->update_gui(_current_config);
    qDebug() << QString("update_gui time ") << timer.elapsed();
    config_changed();
    _server_mutex.unlock();
    _has_request[Http_urls::update_gui_url.c_str()] = 0;
  }

}
