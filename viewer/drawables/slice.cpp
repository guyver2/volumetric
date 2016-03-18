#include "slice.hpp"
#include "volume.hpp"
#include <iostream>
#include <vector>

#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(const char* file, int line)
{
  
  GLenum glErr;
  int    retCode = 0;
  
  glErr = glGetError();
  if (glErr != GL_NO_ERROR)
  {
    printf("glError in file %s @ line %d: %s\n",
           file, line, gluErrorString(glErr));
    retCode = 1;
  }
  return retCode;
}



//std::ostream& operator<<(std::ostream &, const shape&);

Slice::Slice(Volume<unsigned char> *vol) : Drawable(),
                            _volume(vol),
                            _alpha(0.9),
                            _slices(std::vector<int>(3,50)),
                            _visible(std::vector<bool>(3,true)),
                            _textures(std::vector<GLuint>(3,0)),
                            _maxIntTextures(std::vector<GLuint>(3,0)),
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

void Slice::setAlpha(float a)
{
  _alpha = a;
  refresh();
}

float Slice::getAlpha()
{
  return _alpha;
}

void Slice::setMaxInt(int a)
{
  _showMaxInt = a;
  refresh();
}

bool Slice::getMaxInt()
{
  return _showMaxInt;
}


int Slice::getSlice(char s)
{
  switch (s) {
    case 'w':
      return _slices[0];
    case 'h':
      return _slices[1];
    case 'd':
      return _slices[2];
    default:
      std::cerr << "Error : Unknown axis : " << s << " for 3d volume in 'getSlice'" << std::endl;
      return 0;
  }
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
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(_alpha, _alpha, _alpha, _alpha);
  // X slice
  if (_visible[0]){
    if (_textures[0] == 0) {
      auto tmp = _volume->getSlice(_slices[0], 'w');
      _textures[0] = sliceToTexture(tmp);
    }
    glBindTexture(GL_TEXTURE_2D, _textures[0]);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex3f(x,0,0);
    glTexCoord2d(0,1);
    glVertex3f(x,s.h,0);
    glTexCoord2d(1,1);
    glVertex3f(x,s.h,s.d);
    glTexCoord2d(1,0);
    glVertex3f(x,0,s.d);
    glEnd();
  }
  // Y slice
  if (_visible[1]){
    if (_textures[1] == 0) {
      auto tmp = _volume->getSlice(_slices[1], 'h');
      _textures[1] = sliceToTexture(tmp);
    }
    glBindTexture(GL_TEXTURE_2D, _textures[1]);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex3f(0,y,0);
    glTexCoord2d(0,1);
    glVertex3f(s.w,y,0);
    glTexCoord2d(1,1);
    glVertex3f(s.w,y,s.d);
    glTexCoord2d(1,0);
    glVertex3f(0,y,s.d);
    glEnd();
  }
  // Z slice
  if (_visible[2]){
    if (_textures[2] == 0) {
      auto tmp = _volume->getSlice(_slices[2], 'd');
      _textures[2] = sliceToTexture(tmp);
    }
    glBindTexture(GL_TEXTURE_2D, _textures[2]);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex3f(0,0,z);
    glTexCoord2d(0,1);
    glVertex3f(0,s.h,z);
    glTexCoord2d(1,1);
    glVertex3f(s.w,s.h,z);
    glTexCoord2d(1,0);
    glVertex3f(s.w,0,z);
    glEnd();
  }
  
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  
  glPopMatrix();
}

const shape Slice::getVolumeShape()
{
  return _volume->getShape();
}



void Slice::changeSlice(char s, int id)
{
  if (s == 'w') {
    _slices[0] = id;
    slice2dUC data = _volume->getSlice(id, 'w');
    if (_textures[0]) {
      glDeleteTextures(1, &(_textures[0]));
    }
    _textures[0] = sliceToTexture(data);
  } else if (s == 'h') {
    _slices[1] = id;
    slice2dUC data = _volume->getSlice(id, 'h');
    if (_textures[1]) {
      glDeleteTextures(1, &(_textures[1]));
    }
    _textures[1] = sliceToTexture(data);
  } else if (s == 'd') {
    _slices[2] = id;
    slice2dUC data = _volume->getSlice(id, 'd');
    if (_textures[2]) {
      glDeleteTextures(1, &(_textures[2]));
    }
    _textures[2] = sliceToTexture(data);
  } else {
    std::cerr << "Error : Unknown axis : " << s << " for 3d volume in 'changeSlice'" << std::endl;
  }
  refresh();
}


void Slice::setVisible(int id, int state)
{
  if (id<0 || id>2) {
    std::cerr << "Error : Invalid index for slice in setVisible : " << id << std::endl;
  } else {
    _visible[id] = state;
    refresh();
  }
}

bool Slice::getVisible(int id)
{
  if (id<0 || id>2) {
    std::cerr << "Error : Invalid index for slice in setVisible : " << id << std::endl;
    return false;
  } else {
    return _visible[id];
  }
}


void Slice::buildMaxIntensityTextures()
{
  // max intensity on X
  
}


GLuint Slice::sliceToTexture(slice2dUC &slice)
{
  GLuint texId;
  glGenTextures(1, &texId);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, texId);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, slice.h, slice.w, 0,
               GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);//static_cast<void*>(slice.data.data()));
  glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0,
                   slice.h,
                   slice.w,
                   GL_LUMINANCE,
                   GL_UNSIGNED_BYTE,
                   static_cast<void*>(slice.data.data()));
  return texId;
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
                   _sliderA(new QSlider(Qt::Horizontal, this)),
                   _labelA(new QLabel("Alpha :", this)),
                   _layout(new QGridLayout(this))
{
  _checkX->setCheckState(static_cast<Slice*>(_drawable)->getVisible(0)?Qt::Checked:Qt::Unchecked);
  _checkY->setCheckState(static_cast<Slice*>(_drawable)->getVisible(1)?Qt::Checked:Qt::Unchecked);
  _checkZ->setCheckState(static_cast<Slice*>(_drawable)->getVisible(2)?Qt::Checked:Qt::Unchecked);
  _maxInt->setCheckState(static_cast<Slice*>(_drawable)->getMaxInt()?Qt::Checked:Qt::Unchecked);
  shape s = static_cast<Slice*>(_drawable)->getVolumeShape();
  _spinX->setRange(0, s.w-1);
  _spinY->setRange(0, s.h-1);
  _spinZ->setRange(0, s.d-1);
  _sliderX->setRange(0, s.w-1);
  _sliderY->setRange(0, s.h-1);
  _sliderZ->setRange(0, s.d-1);
  _sliderA->setRange(0, 100);
  _sliderX->setValue(static_cast<Slice*>(_drawable)->getSlice('w'));
  _spinX->setValue(static_cast<Slice*>(_drawable)->getSlice('w'));
  _sliderY->setValue(static_cast<Slice*>(_drawable)->getSlice('h'));
  _spinY->setValue(static_cast<Slice*>(_drawable)->getSlice('h'));
  _sliderZ->setValue(static_cast<Slice*>(_drawable)->getSlice('d'));
  _spinZ->setValue(static_cast<Slice*>(_drawable)->getSlice('d'));
  _sliderA->setValue((int)(static_cast<Slice*>(_drawable)->getAlpha()*100));
  
  _layout->addWidget(_checkX,0,0);
  _layout->addWidget(_spinX,0,1,1,1, Qt::AlignRight);
  _layout->addWidget(_sliderX,1,0,1,2);
  _layout->addWidget(_checkY,2,0);
  _layout->addWidget(_spinY,2,1,1,1, Qt::AlignRight);
  _layout->addWidget(_sliderY,3,0,1,2);
  _layout->addWidget(_checkZ,4,0);
  _layout->addWidget(_spinZ,4,1,1,1, Qt::AlignRight);
  _layout->addWidget(_sliderZ,5,0,1,2);
  _layout->addWidget(_labelA, 6, 0);
  _layout->addWidget(_sliderA, 6, 1);
  _layout->addWidget(_maxInt,7,0,1,2);
  _layout->addWidget(new QWidget(NULL));
  
  connect(_spinX, SIGNAL(valueChanged(int)), _sliderX, SLOT(setValue(int)));
  connect(_sliderX, SIGNAL(sliderMoved(int)), _spinX, SLOT(setValue(int)));
  connect(_spinY, SIGNAL(valueChanged(int)), _sliderY, SLOT(setValue(int)));
  connect(_sliderY, SIGNAL(sliderMoved(int)), _spinY, SLOT(setValue(int)));
  connect(_spinZ, SIGNAL(valueChanged(int)), _sliderZ, SLOT(setValue(int)));
  connect(_sliderZ, SIGNAL(sliderMoved(int)), _spinZ, SLOT(setValue(int)));
  
  connect(_sliderX, SIGNAL(valueChanged(int)), this, SLOT(changeSliceX(int)));
  connect(_sliderY, SIGNAL(valueChanged(int)), this, SLOT(changeSliceY(int)));
  connect(_sliderZ, SIGNAL(valueChanged(int)), this, SLOT(changeSliceZ(int)));
  connect(_sliderA, SIGNAL(valueChanged(int)), this, SLOT(changeAlpha(int)));
  
  connect(_checkX, SIGNAL(stateChanged(int)), this, SLOT(setSliceVisibleX(int)));
  connect(_checkY, SIGNAL(stateChanged(int)), this, SLOT(setSliceVisibleY(int)));
  connect(_checkZ, SIGNAL(stateChanged(int)), this, SLOT(setSliceVisibleZ(int)));
  connect(_maxInt, SIGNAL(stateChanged(int)), this, SLOT(setMaxInt(int)));
  
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
  delete _sliderA;
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
  static_cast<Slice*>(_drawable)->changeSlice('d', id);
}

void SliceControlPanel::changeAlpha(int id)
{
  static_cast<Slice*>(_drawable)->setAlpha(id/100.);
}

void SliceControlPanel::setSliceVisibleX(int state)
{
  static_cast<Slice*>(_drawable)->setVisible(0, state);
}

void SliceControlPanel::setSliceVisibleY(int state)
{
  static_cast<Slice*>(_drawable)->setVisible(1, state);
}

void SliceControlPanel::setSliceVisibleZ(int state)
{
  static_cast<Slice*>(_drawable)->setVisible(2, state);
}

void SliceControlPanel::setMaxInt(int state)
{
  static_cast<Slice*>(_drawable)->setMaxInt(state);
}

