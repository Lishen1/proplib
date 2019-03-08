#include "proplibqtgui.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QTextStream>
#include "QElapsedTimer"
#include <iostream>

int main(int argc, char *argv[])
{


  QApplication a(argc, argv);

  //QFile f("D:/User_data/VC_PROJECTS/proplib/proplib-gui/proplib-qt-gui/qdarkstyle/style.qss");
  //if (!f.exists())
  //{
  //  printf("Unable to set stylesheet, file not found\n");
  //}
  //else
  //{
  //  f.open(QFile::ReadOnly | QFile::Text);
  //  QTextStream ts(&f);
  //  qApp->setStyleSheet(ts.readAll());
  //}

  proplibqtgui w;

  YAML::Node n = YAML::LoadFile(R"(D:\User_data\VC_PROJECTS\proplib\tests\prop-serialize\test.yml)");
  QElapsedTimer timer;
  timer.start();
  w.build_gui(n);
  w.update_gui(n);
  std::cout << "build_gui time " << timer.elapsed() << std::endl;

  w.show();

  return a.exec();
}