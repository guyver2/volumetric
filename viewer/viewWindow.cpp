
#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>

#include "viewWindow.hpp"
#include "viewer.hpp"
#include "drawables/drawable.hpp"

ViewerWindow::ViewerWindow() : QMainWindow(),
                               _viewer(new Viewer(this)),
                               _splitter(new QSplitter(this))
{
  _controls = new QTabWidget(this);
  _viewer->installEventFilter(this);
  _viewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  _splitter->addWidget(_viewer);
  _splitter->addWidget(_controls);
  this->setCentralWidget(_splitter);
  this->resize(1000, 800);
}

ViewerWindow::~ViewerWindow()
{
  delete _controls;
  delete _viewer;
  delete _splitter;
}


void ViewerWindow::addDrawable(std::string name, Drawable* d)
{
  name = _viewer->addDrawable(name, d);
  _controls->addTab(d->controlPanel(), QString(name.c_str()));
  _controls->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

