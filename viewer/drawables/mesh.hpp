#ifndef MESH_DRAWABLE_H
#define MESH_DRAWABLE_H


#include <QLabel>
#include <QCheckBox>
#include <QToolButton>
#include <QGridLayout>
#include <QColor>

#include <vector>
#include <string>

#include "drawable.hpp"
#include "../util.h"

class Mesh
{
  public :
    Mesh(const std::string&);
    ~Mesh();
    std::vector<float> _vertices;
    std::vector<uint>  _faces;
};

//------------------------------------------------------------------------------

class MeshDrawable : public Drawable
{
public :
  MeshDrawable(const std::string&);
  ~MeshDrawable();
  virtual void draw(); // draw content
  int getVisibleEdges();
  void setVisibleEdges(int);
  color getEdgesColor();
  void setEdgesColor(color);
  color getFacesColor();
  void setFacesColor(color);
  
private :
  void updateSizeAndCenter();
  Mesh _mesh;
  color _edgesColor;
  color _facesColor;
  bool _visibleEdges;
};



class MeshDrawableControlPanel : public ControlPanel
{
  Q_OBJECT
public :
  MeshDrawableControlPanel(Drawable*, QWidget* parent=NULL);
  ~MeshDrawableControlPanel();

public slots:
  virtual void changeEdgesColor();
  virtual void changeFacesColor();
  virtual void setVisibleEdges(int);

private :
  QCheckBox*   _checkEdges;
  QToolButton* _butEdgesColor;
  QLabel*      _labelEdgesColor;
  QToolButton* _butFacesColor;
  QLabel*      _labelFacesColor;
  QGridLayout* _layout;
};


//------------------------------------------------------------------------------

class MeshSeqDrawable : public Drawable
{
public :
  MeshSeqDrawable(const std::vector<std::string>&);
  ~MeshSeqDrawable();
  virtual void draw(); // draw content
  
private :
  void updateSizeAndCenter();
  std::vector<Mesh> _meshes;
  uint _id;
  color _color;
};



#endif // MESH_DRAWABLE_H