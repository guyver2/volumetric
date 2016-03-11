#ifndef VIEWER_H
#define VIEWER_H

#include "util.h"
#include "arcball.hpp"
#include "drawables/drawable.hpp"

#include <QtOpenGL/QGLWidget>

typedef std::map<std::string, Drawable*>::const_iterator DrawableIterator;

class Viewer : public QGLWidget
{
  Q_OBJECT
  
  public :
    Viewer(QWidget*);
    ~Viewer();
    virtual void paintGL();
    void drawCross(float);
    virtual void resizeGL(int, int);
    virtual void wheelEvent(QWheelEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void keyPressEvent(QKeyEvent*);
    void zoom(float);
    std::string addDrawable(std::string, Drawable*);

  private :
    QWidget* _parent;
    color    _bgColor;
    color    _crossColor;
    bool     _showCross;
    p3d      _pos;
    p3d      _focus;
    float    _fov;
    float    _radius;
    float    _zfar;
    Arcball  _arcball;
    int      _oldMouseX;
    int      _oldMouseY;
    std::map<std::string, Drawable*> _drawables;
};


#endif // VIEWER_H
