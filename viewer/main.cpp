#include <QApplication>
#include "viewer.hpp"


int main(int argv, char **args)
{
  QApplication app(argv, args);
  Viewer viewer;
  viewer.show();
  return app.exec();
}