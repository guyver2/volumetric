#include "viewer.hpp"
#include "arcball.hpp"
#include "drawables/drawable.hpp"

#include <QtCore>
#include <QtOpenGL/QGLWidget>
#include <QWheelEvent>
#include <GL/glu.h>
#include <GL/gl.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <cmath>

Viewer::Viewer(QWidget* parent, int W, int H): QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
                  _parent(parent),
                  _bgColor({0.3,0.3,0.3,1}),
                  _crossColor({1,1,1,1}),
                  _showCross(true),
                  _pos({0,0,-20}),
                  _focus({0,0,0}),
                  _fov(60),
                  _radius(10),
                  _zfar(200),
                  _arcball(Arcball()),
                  _oldMouseX(0),
                  _oldMouseY(0),
                  _drawables(std::map<std::string, Drawable*>())
{
  _arcball.setWidthHeight(this->width(), this->height());
  this->setFocusPolicy(Qt::StrongFocus);
  this->resize(W, H);
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
  // display all drawable
  for (DrawableIterator iter = _drawables.begin(); iter != _drawables.end(); iter++){
    //std::cout << "Key: " << iter->first << std::endl;
    iter->second->draw();
  }
}

// draw a 3D cross that shows world origin / scale / orientation
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


std::string Viewer::addDrawable(std::string name, Drawable *d)
{
  connect(d, SIGNAL(needsRefresh()), this, SLOT(repaint()));
  if (_drawables.count(name)){ // name already present, rename it
    std::string newName = std::to_string(_drawables.size())+name;
    _drawables[newName] = d;
    focusAll();
    return newName;
  } else {
    _drawables[name] = d;
    focusAll();
    return name;
  }
}

void Viewer::focusAll()
{
  if (_drawables.size() == 0){
    focusOn({0,0,0}, 50);
  } else {
    std::vector<float> Xs;
    std::vector<float> Ys;
    std::vector<float> Zs;
    for (DrawableIterator iter = _drawables.begin(); iter != _drawables.end(); iter++){
      Drawable *d = iter->second;
      Xs.push_back(d->center().x-d->radius());
      Xs.push_back(d->center().x+d->radius());
      Ys.push_back(d->center().y-d->radius());
      Ys.push_back(d->center().y+d->radius());
      Zs.push_back(d->center().z-d->radius());
      Zs.push_back(d->center().z+d->radius());
    }
    std::vector<float>::iterator xmin = std::min_element(std::begin(Xs), std::end(Xs));
    std::vector<float>::iterator xmax = std::max_element(std::begin(Xs), std::end(Xs));
    std::vector<float>::iterator ymin = std::min_element(std::begin(Ys), std::end(Ys));
    std::vector<float>::iterator ymax = std::max_element(std::begin(Ys), std::end(Ys));
    std::vector<float>::iterator zmin = std::min_element(std::begin(Zs), std::end(Zs));
    std::vector<float>::iterator zmax = std::max_element(std::begin(Zs), std::end(Zs));
    
    float radius = fmax(fmax(*xmax- *xmin, *ymax-*ymin), *zmax - *zmin);
    p3d newCenter = {(*xmax+ *xmin)/2.,(*ymax+ *ymin)/2.,(*zmax+ *zmin)/2.};
    focusOn(newCenter, radius);
  }
}
void Viewer::focusOn(p3d center, float radius)
{
  //std::cout << "focusing on (" << center.x << ", " << center.y << ", " << center.z << ") "
  //          << "with radius : " << radius << std::endl;
  _focus = center;
  _radius = radius;
  _zfar = 10*radius;
  _pos.z = -1.5*radius;
  this->updateGL();
}

