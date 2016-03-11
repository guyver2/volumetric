#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "viewer.hpp"
#include <QtCore>

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

private :
  int _priority;
  p3d _center;
  float _radius;
  bool _active;
  //ControlPanel _controlPanel;
};



class ControlPanel : public QWidget{

public :
      ControlPanel(Drawable*, QWidget* parent=NULL);
      ~ControlPanel();
      virtual void setActive(bool);

private :
    Drawable* _drawable;
    QWidget*  _parent;
};

#endif // DRAWABLE_H