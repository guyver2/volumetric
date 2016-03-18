#include "slice.hpp"
#include "volume.hpp"
#include <iostream>
#include <vector>



//std::ostream& operator<<(std::ostream &, const shape&);

Slice::Slice(Volume<unsigned char> *vol) : Drawable(),
                            _volume(vol),
                            _slices(std::vector<int>(3,0)),
                            _visible(std::vector<bool>(3,true)),
                            _showMaxInt(false)
{
  delete _controlPanel;
  _controlPanel = new SliceControlPanel(this);
  const shape s = _volume->getShape();
  _center = {0,0,0};
  _radius = fmax(fmax(s.w, s.h), s.d);
}

Slice::~Slice()
{
}

void Slice::draw()
{
  drawBoundingBox();
  drawSlice();
}

void Slice::drawBoundingBox()
{
  glLineStipple(1, 0x000F);
  glEnable(GL_LINE_STIPPLE);
  glColor4f(1,1,1,1);
  const shape s = _volume->getShape();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(s.w/-2., s.h/-2., s.d/-2.);
  // corners
  float vertices[] = {       0,          0,          0,
                    (float)s.w,          0,          0,
                    (float)s.w, (float)s.h,          0,
                             0, (float)s.h,          0,
                             0,          0, (float)s.d,
                    (float)s.w,          0, (float)s.d,
                    (float)s.w, (float)s.h, (float)s.d,
                             0, (float)s.h, (float)s.d};
  // edges list
  uchar indices[] = {0,1, 1,2, 2,3, 3,0, 4,5, 5,6, 6,7, 7,4, 0,4, 1,5, 2,6, 3,7};
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertices);
  glDrawElements(GL_LINES, 24, GL_UNSIGNED_BYTE, indices);
  glDisableClientState(GL_VERTEX_ARRAY);
  
  glPopMatrix();
  glDisable(GL_LINE_STIPPLE);
}


void Slice::drawSlice()
{
  const shape s = _volume->getShape();
  
  int x = _slices[0];
  int y = _slices[1];
  int z = _slices[2];
  
  std::vector<unsigned char> Xdata = _volume->getSlice(x, 'w');
  std::vector<unsigned char> Ydata = _volume->getSlice(y, 'h');
  std::vector<unsigned char> Zdata = _volume->getSlice(y, 'd');
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(s.w/-2., s.h/-2., s.d/-2.);
  // draw borders
  // X plane
  if (_visible[0]){
    glColor4f(1,0,0,1);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x,0,0);
    glVertex3f(x,s.h,0);
    glVertex3f(x,s.h,s.d);
    glVertex3f(x,0,s.d);
    glEnd();
  }
  // Y plane
  if (_visible[1]){
    glColor4f(0,1,0,1);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0,y,0);
    glVertex3f(s.w,y,0);
    glVertex3f(s.w,y,s.d);
    glVertex3f(0,y,s.d);
    glEnd();
  }
  // Z plane
  if (_visible[2]){
    glColor4f(0,0,1,1);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0,0,z);
    glVertex3f(0,s.h,z);
    glVertex3f(s.d,s.h,z);
    glVertex3f(s.d,0,z);
    glEnd();
  }
  glPopMatrix();
}

const shape Slice::getVolumeShape()
{
  std::cout << "volume points to :" << _volume << std::endl;
  return _volume->getShape();
}



void Slice::changeSlice(char s, int id)
{
  if (s == 'w') {
    _slices[0] = id;
  } else if (s == 'h') {
    _slices[1] = id;
  } else if (s == 'd') {
    _slices[2] = id;
  }
  refresh();
}


//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------



SliceControlPanel::SliceControlPanel(Drawable* d, QWidget* parent) :
                   ControlPanel(d, parent),
                   _checkX(new QCheckBox("X Slice", this)),
                   _checkY(new QCheckBox("Y Slice", this)),
                   _checkZ(new QCheckBox("Z Slice", this)),
                   _maxInt(new QCheckBox("Maximum intensity", this)),
                   _spinX(new QSpinBox(this)),
                   _spinY(new QSpinBox(this)),
                   _spinZ(new QSpinBox(this)),
                   _sliderX(new QSlider(Qt::Horizontal, this)),
                   _sliderY(new QSlider(Qt::Horizontal, this)),
                   _sliderZ(new QSlider(Qt::Horizontal, this)),
                   _layout(new QGridLayout(this))
{
  _checkX->setCheckState(Qt::Checked);
  _checkY->setCheckState(Qt::Checked);
  _checkZ->setCheckState(Qt::Checked);
  _maxInt->setCheckState(Qt::Unchecked);
  shape s = static_cast<Slice*>(_drawable)->getVolumeShape();
  std::cout << s << std::endl;
  _spinX->setRange(0, s.w-1);
  _spinY->setRange(0, s.h-1);
  _spinZ->setRange(0, s.d-1);
  
  _layout->addWidget(_checkX,0,0);
  _layout->addWidget(_spinX,0,1,1,1, Qt::AlignRight);
  _layout->addWidget(_sliderX,1,0,1,2);
  _layout->addWidget(_checkY,2,0);
  _layout->addWidget(_spinY,2,1,1,1, Qt::AlignRight);
  _layout->addWidget(_sliderY,3,0,1,2);
  _layout->addWidget(_checkZ,4,0);
  _layout->addWidget(_spinZ,4,1,1,1, Qt::AlignRight);
  _layout->addWidget(_sliderZ,5,0,1,2);
  _layout->addWidget(_maxInt,6,0,1,2);
  _layout->addWidget(new QWidget(NULL));
  
  connect(_spinX, SIGNAL(valueChanged(int)), _sliderX, SLOT(setValue(int)));
  connect(_sliderX, SIGNAL(sliderMoved(int)), _spinX, SLOT(setValue(int)));
  connect(_spinY, SIGNAL(valueChanged(int)), _sliderY, SLOT(setValue(int)));
  connect(_sliderY, SIGNAL(sliderMoved(int)), _spinY, SLOT(setValue(int)));
  connect(_spinZ, SIGNAL(valueChanged(int)), _sliderZ, SLOT(setValue(int)));
  connect(_sliderZ, SIGNAL(sliderMoved(int)), _spinZ, SLOT(setValue(int)));
  
  connect(_sliderX, SIGNAL(valueChanged(int)), static_cast<SliceControlPanel*>(this), SLOT(changeSliceX(int)));
  connect(_sliderY, SIGNAL(valueChanged(int)), this, SLOT(changeSliceY(int)));
  connect(_sliderZ, SIGNAL(valueChanged(int)), this, SLOT(changeSliceY(int)));
  
  this->setLayout(_layout);
}


SliceControlPanel::~SliceControlPanel()
{
  delete _layout;
  delete _checkX;
  delete _checkY;
  delete _checkZ;
  delete _maxInt;
  delete _spinX;
  delete _spinY;
  delete _spinZ;
  delete _sliderX;
  delete _sliderY;
  delete _sliderZ;
}


void SliceControlPanel::changeSliceX(int id)
{
  static_cast<Slice*>(_drawable)->changeSlice('w', id);
}

void SliceControlPanel::changeSliceY(int id)
{
  static_cast<Slice*>(_drawable)->changeSlice('h', id);
}

void SliceControlPanel::changeSliceZ(int id)
{
  static_cast<Slice*>(_drawable)->changeSlice('z', id);
}




