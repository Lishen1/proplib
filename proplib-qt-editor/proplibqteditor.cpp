#include "proplibqteditor.h"
#include <proplibqtgui.h>
#include "QElapsedTimer"
#include <QFileDialog>
#include <iostream>
#include <strstream>
#include "QAbstractScrollArea"
#include "QScrollBar"
#include "Yaml_highlighter.h"

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

    //startTimer(5000);

    QObject::connect(ui.actionOpen_Config, &QAction::triggered, this, &proplibqteditor::open_config_diag);
    QObject::connect(_yaml_viewer, &QTextEdit::cursorPositionChanged, this, &proplibqteditor::cursor_position_changed);

    open_config(R"(D:\User_data\VC_PROJECTS\proplib\tests\prop-serialize\test.yml)");

}


void proplibqteditor::open_config(const QString& path)
{
  _current_config = YAML::LoadFile(path.toLatin1().constData());
  QElapsedTimer timer;
  timer.start();
  _prop_gui->build_gui(_current_config);
  //_prop_gui->build_gui(yaml_config);

  _prop_gui->update_gui(_current_config);
  std::cout << "build_gui time " << timer.elapsed() << std::endl;

  config_changed();
}

void proplibqteditor::open_config_diag(const bool& on)
{
  QString file_name = QFileDialog::getOpenFileName(this,
    tr("Open Config"), R"(D:\User_data\VC_PROJECTS\proplib\tests\prop-serialize)", tr("Config Files (*.yml *.yaml)"));

  if(!file_name.isEmpty())
    open_config(file_name);
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
  //config_changed();
}
