#ifndef VIEWER_WINDOW_H
#define VIEWER_WINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>

#include "viewer.hpp"
#include "drawables/drawable.hpp"

class ViewerWindow : public QMainWindow
{
  Q_OBJECT

public:
  ViewerWindow();
  ~ViewerWindow();
  void addDrawable(std::string, Drawable*);

private:
  Viewer*     _viewer;
  QSplitter*  _splitter; // main widget
  QTabWidget* _controls; // side widget with control panels
};


#endif // VIEWER_WINDOW_H