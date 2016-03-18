#ifndef VOLUME_H
#define VOLUME_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>

// shape of a 3D volume
typedef struct shape{
  uint w; // width
  uint h; // height
  uint d; // depth
} shape;

// forward declaration, see Volume.cpp for definition
std::ostream& operator<<(std::ostream &, const shape&);

// shape of a 3D volume
template<typename T>
struct slice2d{
  uint w; // width
  uint h; // height
  std::vector<T> data;
};

template <typename T>
class Volume{

public :
  Volume<T>(); // constructor
  Volume(shape); // constructor with given size, fill with zeros
  ~Volume(); // destructor
  const shape getShape() // accessor
  {
    return _shape;
  }
  shape fromFile(std::string); // load volume from file
  void toFile(std::string); // save Volume to file
  shape fromData(const std::vector<T>&, shape); // load volume from data
  struct slice2d<T> getSlice(uint, char); // get a 2D slice along a given dimension
  // accessor
  T& operator()(uint, uint, uint);
  //const T& operator()(uint, uint, uint);

private :
  std::vector<T>  _data;
  shape           _shape;
};




// default constructor, empty data
template <typename T>
Volume<T>::Volume(): _data(std::vector<T>()),
_shape({0,0,0})
{
}

// default constructor with given shape, fill data with zeros
template <typename T>
Volume<T>::Volume(shape s)
{
  _shape = s;
  _data = std::vector<T>(s.w*s.h*s.d, 0);
}

// default destructor
template <typename T>
Volume<T>::~Volume()
{
}


// convininent accessor with parenthesis
template <typename T>
T& Volume<T>::operator()(uint x, uint y, uint z)
{ // check if value is in range
  if (x>=_shape.w || y>=_shape.h || z>=_shape.d) {
    std::cerr << "error accessing (" << x << ", " << y << ", " << z
    << ") out of bound (" << _shape.w << ", " << _shape.h << ", "
    << _shape.d << ")" << std::endl;
  }
  return _data[z*(_shape.w*_shape.h)+y*_shape.w+x];
}

// load data from file
template <typename T>
shape Volume<T>::fromFile(std::string filename)
{
  std::ifstream fd(filename, std::ios::in);
  // read shape
  fd >> _shape.w >> _shape.h >> _shape.d;
  // read data
  _data.clear();
  double d;
  while (fd >> d){
    _data.push_back((T)d);
  }
  fd.close();
  // check if data is consistent with the shape
  if (_data.size() != _shape.w*_shape.h*_shape.d){
    std::cout << "error loading file " << filename
    << ". Shape do not agree with data size. \n(" << _shape.w << ", "
    << _shape.h << ", " << _shape.d << "):" << _shape.w*_shape.h*_shape.d 
    << " != " << _data.size() << std::endl;
    _shape = {0,0,0};
    _data.clear();
  }
  return _shape;
}


// write data to file
template <typename T>
void Volume<T>::toFile(std::string filename)
{
  std::ofstream fd(filename, std::ios::out);
  fd << _shape.w << ' ' << _shape.h << ' ' << _shape.d << '\n';
  std::ostream_iterator<double> out_it (fd," ");
  std::copy ( _data.begin(), _data.end(), out_it );
  fd.close();
}


// read data from a buffer
template <typename T>
shape Volume<T>::fromData(const std::vector<T> &data, shape s)
{
  // check if data and shape are consistent
  if (data.size() != s.w*s.h*s.d){
    std::cout << "error loading data "
    << ". Shape do not agree with data size. \n(" << s.w << ", "
    << s.h << ", " << s.d << "):" << s.w*s.h*s.d 
    << " != " << data.size() << std::endl;
    _shape = {0,0,0};
    _data.clear();
  } else {
    _shape = s;
    _data = data;
  }
  return _shape;
}


// return a 2D slice of the volume from an axis and a slice ID
template <typename T>
struct slice2d<T> Volume<T>::getSlice(uint id, char axis)
{
  struct slice2d<T> res;
  switch(axis){
    case 'w':
      res.w = _shape.d;
      res.h = _shape.h;
      for(int i=0; i<_shape.h; i++){
        for (int j=0; j<_shape.d; j++){
          res.data.push_back(_data.at((j*_shape.w*_shape.h)+(i*_shape.w)+id));
        }
      }
      break;
    case 'h':
      res.w = _shape.d;
      res.h = _shape.w;
      for(int i=0; i<_shape.d; i++){
        auto start = _data.begin()+(i*_shape.w*_shape.h) + id*_shape.w;
        auto end = start+_shape.w;
        res.data.insert(res.data.end(), start, end);
      }
      break;
    case 'd':
      res.w = _shape.w;
      res.h = _shape.h;
      res.data = std::vector<T>(_data.begin()+(id*_shape.w*_shape.h), 
                           _data.begin()+((id+1)*_shape.w*_shape.h));
      break;
    default:
      std::cerr << "Cannot get a slice of unknown axis: " << axis
      << ". Please use 'w', 'h' or 'd'" << std::endl;
  }
  return res;
}







#endif // VOLUME_H
