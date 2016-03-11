#ifndef VIEWER_H
#define VIEWER_H

#include "arcball.hpp"

#include <QtOpenGL/QGLWidget>

typedef struct p3d{
  double x;
  double y;
  double z;
} p3d;

typedef struct color{
  float r;
  float g;
  float b;
  float a;
} color;


class Viewer : public QGLWidget
{
  Q_OBJECT
  
  public :
    Viewer();
    ~Viewer();
    virtual void paintGL();
    void drawCross(float);
    virtual void  resizeGL(int, int);
    virtual void wheelEvent(QWheelEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void keyPressEvent(QKeyEvent*);
    void zoom(float);

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
};


#endif // VIEWER_H
