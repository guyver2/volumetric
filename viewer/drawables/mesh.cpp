#include "mesh.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <QColorDialog>


#include <GL/gl.h>

Mesh::Mesh(const std::string& filename): _vertices(std::vector<float>()),
                                         _faces(std::vector<uint>()){
  // load mesh
  std::string buffer;
  std::ifstream file_in(filename.c_str());
  if(!file_in.is_open()){
    std::cerr << "Error : cannot open file "<< filename << std::endl;
    return;
  }
  
  std::getline(file_in,buffer);
  uint type = 0;
  if (buffer.compare("OFF") == 0){
    //std::cout << "regular OFF file" << std::endl;
    type = 0;
  }
  else if (buffer.compare("NOFF") == 0){
    //std::cout << "OFF file with normal" << std::endl;
    type = 1;
  }
  else{
    std::cerr << "Error : "<< filename << "is not an off file"<< std::endl;
    return;
  }
  
  int nbFaces;
  int nbVertices;
  int tmp;
  float tmp2;
  
  file_in >> nbVertices >> nbFaces>> tmp;
  //std::cout << "vert :" << nbVertices << ", and faces :" << nbFaces << std::endl;
  _vertices.resize(3*nbVertices);
  _faces.resize(3*nbFaces);
  for (int i=0; i<nbVertices; i++){
    file_in >> _vertices[3*i] >> _vertices[3*i+1] >> _vertices[3*i+2];
    if (type == 1) file_in >> tmp2 >> tmp2 >> tmp2;
    std::getline(file_in,buffer); // comments maybe
    //std::cout << "vert #" << i << " read extra : " << buffer << std::endl;
  }
  for (int i=0; i<nbFaces; i++){
    // first read a 3, because we only have triangles
    file_in >> tmp >> _faces[3*i] >> _faces[3*i+1] >> _faces[3*i+2];
    std::getline(file_in,buffer); // comments maybe
    //std::cout << "read extra : " << buffer << std::endl;
  }
  file_in.close();
}


Mesh::~Mesh()
{
}



//------------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------------




MeshDrawable::MeshDrawable(const std::string& filename) : Drawable(),
                                                          _mesh(Mesh(filename)),
                                                          _visibleEdges(true){
  delete _controlPanel;
  _edgesColor = {0.9,0.1,0.1,1};
  _facesColor = {0.1,0.7,0.7,1};
  _controlPanel = new MeshDrawableControlPanel(this);
  updateSizeAndCenter();
}


void MeshDrawable::updateSizeAndCenter(){
  int nbVertices = _mesh._vertices.size()/3;
  float minx, maxx, miny, maxy, minz, maxz;
  minx = maxx = _mesh._vertices[0];
  miny = maxy = _mesh._vertices[1];
  minz = maxz = _mesh._vertices[2];
  for (int i=0;i<nbVertices; i++){
    minx = std::min(minx, _mesh._vertices[3*i]);
    maxx = std::max(maxx, _mesh._vertices[3*i]);
    miny = std::min(miny, _mesh._vertices[3*i+1]);
    maxy = std::max(maxy, _mesh._vertices[3*i+1]);
    minz = std::min(minz, _mesh._vertices[3*i+2]);
    maxz = std::max(maxz, _mesh._vertices[3*i+2]);
  }
  _center.x += (maxx+minx)/2;
  _center.y += (maxy+miny)/2;
  _center.z += (maxz+minz)/2;
  _radius = std::max(std::max(maxx-minx, maxy-miny), maxz-minz);
}


MeshDrawable::~MeshDrawable(){
  
}


void MeshDrawable::draw(){
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(-_center.x, -_center.y, -_center.z);
  glScalef(_radius, _radius, _radius);
  glColor4f(_facesColor.r, _facesColor.g, _facesColor.b, _facesColor.a);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, &(_mesh._vertices[0]));
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawElements(GL_TRIANGLES, _mesh._faces.size(), GL_UNSIGNED_INT, &(_mesh._faces[0]));
  if(_visibleEdges){
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor4f(_edgesColor.r, _edgesColor.g, _edgesColor.b, _edgesColor.a);
    glDrawElements(GL_TRIANGLES, _mesh._faces.size(), GL_UNSIGNED_INT, &(_mesh._faces[0]));
  }
  glDisableClientState(GL_VERTEX_ARRAY);
  glPopMatrix();
}


int MeshDrawable::getVisibleEdges(){
  return _visibleEdges;
}

void MeshDrawable::setVisibleEdges(int v){
 _visibleEdges = v;
 refresh();
}

color MeshDrawable::getEdgesColor(){
  return _edgesColor;
}

void MeshDrawable::setEdgesColor(color c){
  _edgesColor = c;
  refresh();
}

color MeshDrawable::getFacesColor(){
  return _facesColor;
}

void MeshDrawable::setFacesColor(color c){
  _facesColor = c;
  refresh();
}
//------------------------------------------------------------------------------
MeshDrawableControlPanel::MeshDrawableControlPanel(Drawable* d, QWidget* parent):
      ControlPanel(d, parent),
      _checkEdges(new QCheckBox("Show Edges", this)),
      _butEdgesColor(new QToolButton(this)),
      _butFacesColor(new QToolButton(this)),
      _labelEdgesColor(new QLabel("Edges Color", this)),
      _labelFacesColor(new QLabel("Faces Color", this)),
      _layout(new QGridLayout(this)){
        
  _checkEdges->setCheckState(dynamic_cast<MeshDrawable*>(_drawable)->getVisibleEdges()?Qt::Checked:Qt::Unchecked);
  QPixmap px = QPixmap(20, 20);
  color c = dynamic_cast<MeshDrawable*>(_drawable)->getEdgesColor();
  px.fill(QColor(c.r*255,c.g*255,c.b*255));
  _butEdgesColor->setIcon(px);
  _butEdgesColor->setFixedSize(20,20);
  c = dynamic_cast<MeshDrawable*>(_drawable)->getFacesColor();
  px.fill(QColor(c.r*255,c.g*255,c.b*255));
  _butFacesColor->setIcon(px);
  _butFacesColor->setFixedSize(20,20);
  
  _layout->addWidget(_checkEdges,0,0, 1, 2, Qt::AlignLeft);
  _layout->addWidget(_butEdgesColor,1,0);
  _layout->addWidget(_labelEdgesColor,1,1);
  _layout->addWidget(_butFacesColor,2,0);
  _layout->addWidget(_labelFacesColor,2,1);
  _layout->addWidget(new QWidget(NULL));
  
  connect(_checkEdges, SIGNAL(stateChanged(int)), this, SLOT(setVisibleEdges(int)));
  connect(_butEdgesColor, SIGNAL(clicked()), this, SLOT(changeEdgesColor()));
  connect(_butFacesColor, SIGNAL(clicked()), this, SLOT(changeFacesColor()));
  
  this->setLayout(_layout);
}

MeshDrawableControlPanel::~MeshDrawableControlPanel()
{
  delete _checkEdges;
  delete _butEdgesColor;
  delete _butFacesColor;
  delete _labelEdgesColor;
  delete _labelFacesColor;
  delete _layout;
}


void MeshDrawableControlPanel::changeEdgesColor(){
  QColor color = QColorDialog().getColor();
  if (!color.isValid()) return;
  QPixmap px = QPixmap(20, 20);
  px.fill(color);
  _butEdgesColor->setIcon(px);
  dynamic_cast<MeshDrawable*>(_drawable)->setEdgesColor({(float)color.red()/255, (float)color.green()/255, (float)color.blue()/255, 1.});
}

void MeshDrawableControlPanel::changeFacesColor(){
  QColor color = QColorDialog().getColor();
  if (!color.isValid()) return;
  QPixmap px = QPixmap(20, 20);
  px.fill(color);
  _butFacesColor->setIcon(px);
  dynamic_cast<MeshDrawable*>(_drawable)->setFacesColor({(float)color.red()/255, (float)color.green()/255, (float)color.blue()/255, 1.});
}

void MeshDrawableControlPanel::setVisibleEdges(int state){
  dynamic_cast<MeshDrawable*>(_drawable)->setVisibleEdges(state);
}


//------------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------------



MeshSeqDrawable::MeshSeqDrawable(const std::vector<std::string>& filenames) : Drawable(),
                                 _meshes(std::vector<Mesh>()),
                                 _id(0){
  _color = {0.7,0.7,0.7,1};
  int i=0;
  for (auto const &filename : filenames) {
    _meshes.push_back(Mesh(filename));
    std::cout << "\r Loading " << i++ << "/" << filenames.size() << std::flush;
  }
  std::cout << " - DONE." <<std::endl;
}

MeshSeqDrawable::~MeshSeqDrawable(){
  
}


void MeshSeqDrawable::updateSizeAndCenter(){
  if (_meshes.size() == 0) return;
  float minx, maxx, miny, maxy, minz, maxz;
  minx = maxx = _meshes[0]._vertices[0];
  miny = maxy = _meshes[0]._vertices[1];
  minz = maxz = _meshes[0]._vertices[2];
  
  for (auto const &mesh : _meshes) {
    int nbVertices = mesh._vertices.size()/3;
    for (int i=0;i<nbVertices; i++){
      minx = std::min(minx, mesh._vertices[3*i]);
      maxx = std::max(maxx, mesh._vertices[3*i]);
      miny = std::min(miny, mesh._vertices[3*i+1]);
      maxy = std::max(maxy, mesh._vertices[3*i+1]);
      minz = std::min(minz, mesh._vertices[3*i+2]);
      maxz = std::max(maxz, mesh._vertices[3*i+2]);
    }
  }
  _center.x += (maxx+minx)/2;
  _center.y += (maxy+miny)/2;
  _center.z += (maxz+minz)/2;
  _radius = std::max(std::max(maxx-minx, maxy-miny), maxz-minz);
}



void MeshSeqDrawable::draw(){
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(-_center.x, -_center.y, -_center.z);
  glScalef(_radius, _radius, _radius);
  glColor4f(_color.r, _color.g, _color.b, _color.a);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, &(_meshes[_id]._vertices[0]));
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawElements(GL_TRIANGLES, _meshes[_id]._faces.size(), GL_UNSIGNED_INT, &(_meshes[_id]._faces[0]));
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor4f(0,0,0,1);
  glDrawElements(GL_TRIANGLES, _meshes[_id]._faces.size(), GL_UNSIGNED_INT, &(_meshes[_id]._faces[0]));
  glDisableClientState(GL_VERTEX_ARRAY);
  glPopMatrix();
}


/*
self.butColorBBox = QtGui.QToolButton(self)
px = QtGui.QPixmap(15, 15)
px.fill(QtGui.QColor(*[c*255 for c in self.drawable.BBoxColor]))
self.butColorBBox.setIcon(px)
self.butColorBBox.clicked.connect(self.changeColorBBox)
self.butColorBBox.setFixedSize(15,15)
*/
