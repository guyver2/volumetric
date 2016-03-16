#ifndef SHAPES_H
#define SHAPES_H

#include <iostream>
#include "GL/gl.h"

#include "drawable.hpp"
#include "../util.h"



//basic cube
class CubeDrawable : public Drawable
{
public :
  CubeDrawable(p3d pos, float size, color col): Drawable()
  {
    _center = pos;
    _radius = size;
    _color = col;
    // initialize openGL buffers
    //-- corners
    float tmp_v[] = { -1, -1, -1,
                       1, -1, -1,
                       1,  1, -1,
                      -1,  1, -1,
                      -1, -1,  1,
                       1, -1,  1,
                       1,  1,  1,
                      -1,  1,  1};
     _vertices = std::vector<float>(tmp_v, tmp_v + sizeof(tmp_v) / sizeof(float));
     // edges list
     unsigned char tmp_i[] = {0,1, 1,2, 2,3, 3,0, 4,5, 5,6, 6,7, 7,4, 0,4, 1,5, 2,6, 3,7};
     _indices = std::vector<unsigned char>(tmp_i, tmp_i + sizeof(tmp_i) / sizeof(unsigned char));

  }
  void draw()
  {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(_center.x, _center.y, _center.z);
    glScalef(_radius, _radius, _radius);
    glColor4f(_color.r, _color.g, _color.b, _color.a);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &_vertices[0]);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_BYTE, &_indices[0]);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
  }
  
private :
  color _color;
  std::vector<float> _vertices;
  std::vector<unsigned char> _indices;
};

// Y = 0 plane
class WorldPlaneDrawable : public Drawable
{
public:
  WorldPlaneDrawable(float size, int nb, char axis) : Drawable()
  {
    _size = size;
    _radius = 2*nb*size;
    _nb = nb;
    _center = {0,0,0};
    if (axis == 'x' || axis == 'y' || axis == 'z'){
      _axis = axis;
    } else {
      std::cerr << "Error, unknown axis '" << axis <<"', using 'y' axis instead." << std::endl;
      _axis = 'y';
    }
    
  }

  void draw()
  {
    if (!_active) return;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    if (_axis == 'y') glRotatef(90, 1, 0, 0);
    else if (_axis == 'x') glRotatef(90, 0, 1, 0);
    glColor4f(0.8, 0.8, 0.8, 1);
    glBegin(GL_LINES);
    float shift = -_size * _nb/2.;
    // grid
    for(int i=0; i<_nb+1; i++){
      glVertex3f(shift + _size*i,  shift, 0);
      glVertex3f(shift + _size*i, -shift, 0);

      glVertex3f( shift, shift + _size*i, 0);
      glVertex3f(-shift, shift + _size*i, 0);
    }
    glEnd();
    glPopMatrix();
  }
  
private :
  float _size;
  int _nb;
  char _axis;

};

#endif // SHAPfES_H