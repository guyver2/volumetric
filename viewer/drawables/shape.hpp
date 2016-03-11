#ifndef SHAPES_H
#define SHAPES_H

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
/*
# Z = 0 plane
class WorldDrawable(Drawable):
  def __init__(self, size, nb):
    super(WorldDrawable, self).__init__()
    self.size = size
    self.nb = nb
    self.radius = 2*nb*size
    self.center = [0,0,0]

  def draw(self):
    GL.glMatrixMode(GL.GL_MODELVIEW)
    GL.glPushMatrix()
    #GL.glLoadIdentity()
    GL.glColor(*[0.7, 0.7, 0.7])
    GL.glBegin(GL.GL_LINES)
    shift = -self.size * self.nb/2.
    # grid
    for i in xrange(self.nb+1):
      GL.glVertex(shift + self.size*i,  shift, 0)
      GL.glVertex(shift + self.size*i, -shift, 0)

      GL.glVertex( shift, shift + self.size*i, 0)
      GL.glVertex(-shift, shift + self.size*i, 0)

    GL.glEnd()

    # frame
    GL.glLineWidth(1.5)
    GL.glBegin(GL.GL_LINES)
    GL.glColor(*[1, 0, 0])
    GL.glVertex(0,  0, 0)
    GL.glVertex(self.size, 0, 0)
    GL.glColor(*[0, 1, 0])
    GL.glVertex(0,  0, 0)
    GL.glVertex(0, self.size, 0)
    GL.glColor(*[0, 0, 1])
    GL.glVertex(0,  0, 0)
    GL.glVertex(0, 0, self.size)

    GL.glEnd()
    GL.glLineWidth(1)

    GL.glPopMatrix()
*/

#endif // SHAPES_H