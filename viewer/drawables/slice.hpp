#ifndef SLICE_H
#define SLICE_H

#include <QGridLayout>
#include <QCheckBox>
#include <QSpinBox>
#include <QSlider>

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
  const shape getVolumeShape();
  void changeSlice(char, int);


private :
  void drawSlice();
  void drawBoundingBox();
  
  Volume<unsigned char>* _volume;
  std::vector<int> _slices;
  std::vector<bool> _visible;
  bool _showMaxInt;
};



class SliceControlPanel : public ControlPanel 
{
  Q_OBJECT
public :
  SliceControlPanel(Drawable*, QWidget* parent=NULL);
  ~SliceControlPanel();
public slots:
  void changeSliceX(int);
  void changeSliceY(int);
  void changeSliceZ(int);

private :
  QCheckBox* _checkX;
  QCheckBox* _checkY;
  QCheckBox* _checkZ;
  QCheckBox* _maxInt;
  QSpinBox*  _spinX;
  QSpinBox*  _spinY;
  QSpinBox*  _spinZ;
  QSlider*   _sliderX;
  QSlider*   _sliderY;
  QSlider*   _sliderZ;
  QGridLayout* _layout;
};


#endif // SLICE_H