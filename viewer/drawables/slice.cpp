#include <iostream>
#include <vector>

#include "slice.hpp"

std::ostream& operator<<(std::ostream &, const shape&);

Slice::Slice(Volume<unsigned char> *vol) : Drawable(),
                            _slices(std::vector<int>(3,0)),
                            _visible(std::vector<bool>(3,true)),
                            _alpha(1),
                            _showMaxInt(false)
{
  _volume = vol;
  const shape s = _volume->getShape();
  std::cout << "shape : " << "(" << s.w << ", " << s.h << ", " << s.d << ")" << std::endl;
  _center = {0,0,0};
  _radius = fmax(fmax(s.w, s.h), s.d);
}

Slice::~Slice()
{
}

void Slice::draw()
{
  drawBoundingBox();
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
