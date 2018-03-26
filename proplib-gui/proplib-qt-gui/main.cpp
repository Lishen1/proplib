#include "proplibqtgui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{

  QApplication a(argc, argv);
  proplibqtgui w;
  w.show();
  return a.exec();
}