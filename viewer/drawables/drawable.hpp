#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <QtCore>
#include <QWidget>

class Drawable;
class ControlPanel;
#include "util.h"
#include "viewer.hpp"


class Drawable : public QWidget{
  Q_OBJECT
signals:
    void needsRefresh();
  
public :
  Drawable();
  ~Drawable();
  //accessor
  virtual int priority();
  virtual p3d center();
  virtual float radius();
  virtual bool active();
  virtual void setActive(bool);
  virtual void draw(); // draw content
  virtual void refresh(); // emit needsRefresh signal
  virtual ControlPanel* controlPanel();

protected :
  int _priority;
  p3d _center;
  float _radius;
  bool _active;
  ControlPanel* _controlPanel;
};



class ControlPanel : public QWidget{
  Q_OBJECT
public :
      ControlPanel(Drawable*, QWidget* parent=NULL);
      ~ControlPanel();
      virtual void setActive(bool);

protected :
    Drawable* _drawable;
    QWidget*  _parent;
};

#endif // DRAWABLE_H