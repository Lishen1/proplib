#include <httplib.h>
#include <editor/proplibqteditor.h>

#include <QtWidgets/QApplication>

#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile f("qdarkstyle/style.qss");
    if (!f.exists())
    {
      printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
      f.open(QFile::ReadOnly | QFile::Text);
      QTextStream ts(&f);
      qApp->setStyleSheet(ts.readAll());
    }
    proplibqteditor w;
    w.show();
    return a.exec();
}
