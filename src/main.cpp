#include "mainwindow.h"
#include <QApplication>
#include <QtCore>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QTranslator translator;
  auto current_locale = ":/translations/logan_" + QLocale::system().name();
  qDebug() << "lang: " << current_locale;
  translator.load(current_locale);
  a.installTranslator(&translator);

  MainWindow w;
  w.show();

  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      w.openFile(QString(argv[i]));
    }
  }
  return a.exec();
}
