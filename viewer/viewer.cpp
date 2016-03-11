#include "viewer.hpp"
#include "arcball.hpp"

#include <QtCore>
#include <QtOpenGL/QGLWidget>
#include <QWheelEvent>
#include <GL/glu.h>
#include <GL/gl.h>

#include <iostream>
#include <cmath>

Viewer::Viewer(): QGLWidget(),
                  _parent(NULL),
                  _bgColor({0.3,0.3,0.3,1}),
                  _crossColor({1,1,1,1}),
                  _showCross(true),
                  _pos({0,0,-10}),
                  _focus({0,0,0}),
                  _fov(60),
                  _radius(10),
                  _zfar(200),
                  _arcball(Arcball()),
                  _oldMouseX(0),
                  _oldMouseY(0)
{
  _arcball.setWidthHeight(this->width(), this->height());
}

Viewer::~Viewer()
{
}


void Viewer::paintGL()
{
  glClearColor(_bgColor.r, _bgColor.g, _bgColor.b, _bgColor.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(_fov, (float)this->width()/this->height(), 0.1, fmax(200,_zfar));
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(_pos.x, _pos.y, _pos.z);
  _arcball.applyRotationMatrix();
  if (_showCross){
    drawCross(_radius/20.0);
  }
}


void Viewer::drawCross(float s)
{
  glLineStipple(1, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  glColor4f(_crossColor.r, _crossColor.g, _crossColor.b, _crossColor.a);
  //corners
  float vertices[] = { -s,  0,  0,
                        s,  0,  0,
                        0, -s,  0,
                        0,  s,  0,
                        0,  0, -s,
                        0,  0,  s};
  // edges list
  unsigned char indices[] = {0,1,2,3,4,5};
  // colors 
  float colBuf[] = {1,0,0,1,1,0,0,1,
                    0,1,0,1,0,1,0,1,
                    0,0,1,1,0,0,1,1};
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices);
  glColorPointer(4, GL_FLOAT, 0, colBuf);
  glDrawElements(GL_LINES, 6, GL_UNSIGNED_BYTE, indices);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisable(GL_LINE_STIPPLE);
}


// widget resize event
void Viewer::resizeGL(int width, int height)
{
  glViewport(0,0, width, height);
  _arcball.setWidthHeight(width, height);
  this->repaint();
}

// zoom in and out
void Viewer::wheelEvent(QWheelEvent * event)
{
  zoom(event->delta());
  this->repaint();
}

// change field of view to zoom
void Viewer::zoom(float v)
{
  v = v/60.; // prevents from zooming too fast
  _fov += v;
  if (_fov < 1){
    _fov = 1;
  }
  if (_fov > 175){
    _fov = 175;
  }
}


// initialize mouse position
void Viewer::mousePressEvent(QMouseEvent* event)
{
  _oldMouseX = event->x();
  _oldMouseY = event->y();
  if (event->buttons() == Qt::LeftButton){
    _arcball.startRotation(event->x(),event->y());
  }
}

// stops arcball rotation
void Viewer::mouseReleaseEvent(QMouseEvent* event)
{
// use != to get the button ID since the button flag for release is inverted
// http://qt-project.org/doc/qt-4.8/qmouseevent.html#buttons
  if (event->buttons() != Qt::LeftButton){
    _arcball.stopRotation();
  }
}

// mouse motion
void Viewer::mouseMoveEvent(QMouseEvent* event)
{
  int dx = event->x() - _oldMouseX;
  int dy = event->y() - _oldMouseY;
  // rotation
  if (event->buttons() == Qt::LeftButton){
    _arcball.updateRotation(event->x(),event->y());
  } else if (event->buttons() == Qt::RightButton) { 
    // move the camera on X and Y
    _pos.x += dx/10.;
    _pos.y -= dy/10.;
  }
  _oldMouseX = event->x();
  _oldMouseY = event->y();
  this->repaint();
}


// keyboard interactions
void Viewer::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape){
    if (_parent != NULL){
      _parent->close();
    } else {
        this->close();
    }
  } else if (event->key() == Qt::Key_R){
        _arcball.reset();
        _pos = {0,0,-10};
        _fov = 60;
        this->repaint();
  }
}