#ifndef VOLUME_H
#define VOLUME_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>


namespace vol
{
  // shape of a 3D volume
  typedef struct shape{
    uint w; // width
    uint h; // height
    uint d; // depth
  } shape;

  // forward declaration, see Volume.cpp for definition
  std::ostream& operator<<(std::ostream &, const shape&);
  
  template <typename T>
  class Volume{

  public :
    Volume<T>(); // constructor
    Volume(shape); // constructor with given size, fill with zeros
    ~Volume(); // destructor
    const shape getShape(); // accessor
    shape fromFile(std::string); // load volume from file
    void toFile(std::string); // save Volume to file
    shape fromData(const std::vector<T>&, shape); // load volume from data
    std::vector<T> getSlice(uint, char); // get a 2D slice along a given dimension
    // accessor
    T& operator()(uint, uint, uint);
    //const T& operator()(uint, uint, uint);

  private :
    std::vector<T>  _data;
    shape           _shape;
  };
}






#endif // VOLUME_H
