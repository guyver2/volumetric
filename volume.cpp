#include "volume.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>



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
std::vector<T> Volume<T>::getSlice(uint id, char axis)
{
  std::vector<T> res;
  switch(axis){
    case 'w':
      
      break;
    case 'h':
      
      break;
    case 'd':
      res = std::vector<T>(_data.begin()+(id*_shape.w*_shape.h), 
                           _data.begin()+((id+1)*_shape.w*_shape.h));
      break;
    default:
      std::cerr << "Cannot get a slice of unknown axis: " << axis
                << ". Please use 'w', 'h' or 'd'" << std::endl;
  }
  return res;
}


std::ostream& operator<<(std::ostream &strm, const shape &s) {
  return strm << "(" << s.w << ", " << s.h << ", " << s.d << ")";
}


template class Volume<float>;
template class Volume<double>;
template class Volume<int>;
template class Volume<char>;
template class Volume<unsigned char>;
