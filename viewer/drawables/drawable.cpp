#include "drawable.hpp"

#include <QWidget>
#include <iostream>


Drawable::Drawable() : QWidget(),
                       _priority(1),
                       _center({0,0,0}),
                       _radius(10),
                       _active(true)
{
}

Drawable::~Drawable()
{
}

int Drawable::priority()
{
  return _priority;
}

p3d Drawable::center()
{
  return _center;
}

float Drawable::radius()
{
  return _radius;
}

bool Drawable::active()
{
  return _active;
}

void Drawable::setActive(bool v)
{
  _active = v;
}

void Drawable::draw()
{
  std::cout << "drawing" << std::endl;
}

void Drawable::refresh()
{
  emit needsRefresh();
}

/*
 * Default control panel
 */
ControlPanel::ControlPanel(Drawable* drawable, QWidget* parent) :
                          QWidget(),
                          _drawable(drawable),
                          _parent(parent)
{
}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::setActive(bool v)
{
  _drawable->setActive(v);
  this->setEnabled(v);
  _drawable->refresh();
}










