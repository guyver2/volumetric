#ifndef SLICE_H
#define SLICE_H

#include <iostream>
#include <vector>

#include "drawable.hpp"
#include "../../volume.hpp"


class Slice : public Drawable
{
public :
  Slice(Volume<unsigned char>*);
  ~Slice();
  virtual void draw();

private :
  void drawBoundingBox();
  Volume<unsigned char>* _volume;
  std::vector<int> _slices;
  std::vector<bool> _visible;
  float _alpha;
  bool _showMaxInt;

};


#endif // SLICE_H