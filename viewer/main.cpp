#include <QApplication>
#include "viewWindow.hpp"
#include "drawables/shape.hpp"

int main(int argv, char **args)
{
  QApplication app(argv, args);
  ViewerWindow mainWin;
  mainWin.addDrawable("cube1", new CubeDrawable({0,0,0}, 5, {0,1,0,1}));
  mainWin.addDrawable("cube1", new CubeDrawable({-2.5,-2.5,-2.5}, 2, {1,0,0,1}));
  mainWin.addDrawable("world", new WorldPlaneDrawable(1, 10, 'y'));
  mainWin.show();
  return app.exec();
}