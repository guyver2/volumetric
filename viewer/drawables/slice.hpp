#ifndef SLICE_H
#define SLICE_H

#include <QGridLayout>
#include <QCheckBox>
#include <QSpinBox>
#include <QSlider>
#include <QLabel>



#include <iostream>
#include <vector>

#include "drawable.hpp"
#include "../../volume.hpp"

typedef struct slice2d<unsigned char> slice2dUC;


class Slice : public Drawable
{
public :
  Slice(Volume<unsigned char>*);
  ~Slice();
  virtual void draw();
  const shape getVolumeShape();
  void changeSlice(char, int);
  int getSlice(char);
  void setAlpha(float);
  float getAlpha();
  void setVisible(int, int);
  bool getVisible(int);
  void setMaxInt(int);
  bool getMaxInt();


private :
  void drawSlice();
  void drawBoundingBox();
  GLuint sliceToTexture(slice2dUC&);
  void buildMaxIntensityTextures();
  
  Volume<unsigned char>* _volume;
  float _alpha;
  std::vector<int> _slices;
  std::vector<bool> _visible;
  std::vector<GLuint> _textures;
  std::vector<GLuint> _maxIntTextures;
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
  void setSliceVisibleX(int);
  void setSliceVisibleY(int);
  void setSliceVisibleZ(int);
  void setMaxInt(int);
  void changeAlpha(int);

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
  QSlider*   _sliderA;
  QLabel*    _labelA;
  QGridLayout* _layout;
};


#endif // SLICE_H