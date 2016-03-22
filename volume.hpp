#ifndef VOLUME_H
#define VOLUME_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <random>

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
class PerlinNoise3D
{ // from http://cs.nyu.edu/~perlin/noise/
private :
  uint seed;
  std::vector<uint> p;
public :
  PerlinNoise3D(uint s) : seed(s), p(std::vector<uint>(256,0)){
    // fills p with range(0,256)
    std::iota(p.begin(), p.end(), 0);
    // Initialize a random engine with seed
    std::default_random_engine engine(seed);
    // Suffle  using the above random engine
    std::shuffle(p.begin(), p.end(), engine);
    // Duplicate the permutation vector
    p.insert(p.end(), p.begin(), p.end());
  }
  double noise(double x, double y, double z) {
      //std::cout << x << " " << y << " " << z;
      int X = (int)std::floor(x) & 255;                  // FIND UNIT CUBE THAT
      int Y = (int)std::floor(y) & 255;                  // CONTAINS POINT.
      int Z = (int)std::floor(z) & 255;
      x -= std::floor(x);                                // FIND RELATIVE X,Y,Z
      y -= std::floor(y);                                // OF POINT IN CUBE.
      z -= std::floor(z);
      double u = fade(x);                                // COMPUTE FADE CURVES
      double v = fade(y);                                // FOR EACH OF X,Y,Z.
      double w = fade(z);
      int A  = p[X]+Y;
      int AA = p[A]+Z;
      int AB = p[A+1]+Z;      // HASH COORDINATES OF
      int B  = p[X+1]+Y;
      int BA = p[B]+Z;
      int BB = p[B+1]+Z;      // THE 8 CUBE CORNERS,
      
      double res = lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  // AND ADD
                                  grad(p[BA  ], x-1, y  , z   )), // BLENDED
                          lerp(u, grad(p[AB  ], x  , y-1, z   ),  // RESULTS
                               grad(p[BB  ], x-1, y-1, z   ))),// FROM  8
                  lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  // CORNERS
                               grad(p[BA+1], x-1, y  , z-1 )), // OF CUBE
                       lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                            grad(p[BB+1], x-1, y-1, z-1 ))));
      //std::cout << " -> " << res << std::endl;
      return res;
    }
    double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
    double lerp(double t, double a, double b) { return a + t * (b - a); }
    double grad(int hash, double x, double y, double z) {
      int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
      double u = h<8 ? x : y;                 // INTO 12 GRADIENT DIRECTIONS.
      double v = h<4 ? y : h==12||h==14 ? x : z;
      double res = ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
      return res;
    }
};


//------------------------------------------------------------------------------
//
// 3D VOLUME CLASS
//
//------------------------------------------------------------------------------
template <typename T>
class Volume{

public :
  Volume<T>(); // constructor
  Volume(shape); // constructor with given size, fill with zeros
  Volume(shape, uint); // constructor perlin noise with given shape and seed
  ~Volume(); // destructor
  const shape getShape(); // accessor
  shape fromFile(std::string); // load volume from file
  shape fromFile_bin(std::string);
  void toFile(std::string); // save Volume to file
  void toFile_bin(std::string filename);
  shape fromData(const std::vector<T>&, shape); // load volume from data
  struct slice2d<T> getSlice(uint, char); // get a 2D slice along a given dimension
  T& operator()(uint, uint, uint);// accessor
  

private :
  std::vector<T>  _data;
  shape           _shape;
};

//------------------------------------------------------------------------------


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

// default constructor with given shape, fill data with perlin noise
template <typename T>
Volume<T>::Volume(shape s, uint seed)
{
  _shape = s;
  _data = std::vector<T>(s.w*s.h*s.d, 0);
  std::vector<double> _datad(s.w*s.h*s.d, 0);
  PerlinNoise3D perlin = PerlinNoise3D(seed);
  for (int i=0; i<s.w; i++)
    for (int j=0; j<s.h; j++)
      for (int k=0; k<s.d; k++)
        //_datad[k*(_shape.w*_shape.h)+j*_shape.w+i] = perlin.noise(10.*(double)i/s.w,10.*(double)j/s.h,10.*(double)k/s.d);
        (*this)(i,j,k) = static_cast<T>(-255*perlin.noise(10.*(double)i/s.w,10.*(double)j/s.h,10.*(double)k/s.d));
  auto result = std::min_element(_datad.begin(), _datad.end());
  std::cout << "min element : " << (double)*result << '\n';
}

// default destructor
template <typename T>
Volume<T>::~Volume()
{
}

template <typename T>
const shape Volume<T>::getShape() // accessor
{
  return _shape;
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


// load data from file
template <typename T>
shape Volume<T>::fromFile_bin(std::string filename)
{
  std::ifstream fd(filename, std::ios::in | std::ios::binary);
  fd.unsetf(std::ios::skipws);
  // get file size:
  std::streampos fileSize;
  fd.seekg(0, std::ios::end);
  fileSize = fd.tellg();
  fd.seekg(0, std::ios::beg);
  fd.read((char*)&(_shape.w), sizeof(uint));
  fd.read((char*)&(_shape.h), sizeof(uint));
  fd.read((char*)&(_shape.d), sizeof(uint));
  fileSize -= 3*sizeof(uint);
  fileSize = fileSize/sizeof(T);
  std::cout << fileSize << " VS " << _shape.w*_shape.h*_shape.d<< std::endl; 
  
  // check if data is consistent with the shape
  if (fileSize != _shape.w*_shape.h*_shape.d){
    std::cout << "error loading file " << filename
    << ". Shape do not agree with file size. \n" << _shape << ":" << _shape.w*_shape.h*_shape.d 
    << " != " << fileSize << std::endl;
    _shape = {0,0,0};
    _data.clear();
  }
  // load data into vector
  _data.reserve(fileSize);
  //fd.read((char*) &_data[0], fileSize*sizeof(T));
  _data.insert(_data.begin(),std::istream_iterator<T>(fd),std::istream_iterator<T>());
  //std::copy(std::istream_iterator<T>(fd), std::istream_iterator<T>(), std::back_inserter(_data));
  std::cout << "data size " << _data.size() << std::endl;
  if (fd)
    std::cout << "all characters read successfully. " << fd.gcount() << std::endl;
  else
    std::cout << "error: only " << fd.gcount() << " could be read" << std::endl;
  fd.close();
  return _shape;
}


// write human readable data to file
template <typename T>
void Volume<T>::toFile(std::string filename)
{
  std::ofstream fd(filename, std::ios::out);
  fd << _shape.w << ' ' << _shape.h << ' ' << _shape.d << '\n';
  std::ostream_iterator<double> out_it (fd," ");
  std::copy ( _data.begin(), _data.end(), out_it );
  fd.close();
}


// write binary data to file
template <typename T>
void Volume<T>::toFile_bin(std::string filename)
{
  std::ofstream fd(filename, std::ios::out | std::ios::binary);
  fd.write((char*)&(_shape.w), sizeof(uint));
  fd.write((char*)&(_shape.h), sizeof(uint));
  fd.write((char*)&(_shape.d), sizeof(uint));
  fd.write((char*)(_data.data()), _data.size()*sizeof(T));
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
      for(int i=0; i<res.h; i++){
        for (int j=0; j<res.w; j++){
          res.data.push_back((*this)(id, i, j));
        }
      }
      break;
    case 'h':
      res.h = _shape.d;
      res.w = _shape.w;
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
  //std::cout << "slice " << axis << " size : "
  //          << "(" << res.w << ", " << res.h << ")" << std::endl;
  return res;
}





#endif // VOLUME_H
