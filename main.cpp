#include <iostream>
#include <opencv2/opencv.hpp>

#include "volume.hpp"

#include <QApplication>
#include "viewer/viewWindow.hpp"
#include "viewer/drawables/shape.hpp"
#include "viewer/drawables/slice.hpp"


int main(int argc, char** argv )
{
  Volume<unsigned char> volume;//Volume<char>();
  if ( argc != 2 )
  {
    std::cout << "usage: " << argv[0] << " dataFile.3d" << std::endl;
    // default, create a sphere like object at the center with decreasing intensity value
    volume = Volume<unsigned char>({100, 100, 100});
    for (int i=0;i<100;i++) for (int j=0;j<100;j++) for (int k=0;k<100;k++)
    {
      float dist = sqrt((i-50)*(i-50) + (j-50)*(j-50) + (k-50)*(k-50));
      volume(i,j,k) = 255*pow((100-dist)/100,4);
    }
    //volume(1,1,1) = 5;
    volume.toFile("vol.txt");
  } else {
    volume.fromFile(std::string(argv[1]));
  }
  std::cout << volume.getShape() << std::endl;
  
    QApplication app(argc, argv);
    ViewerWindow mainWin;
    //mainWin.addDrawable("cube1", new CubeDrawable({0,0,0}, 5, {0,1,0,1}));
    //mainWin.addDrawable("cube1", new CubeDrawable({-2.5,-2.5,-2.5}, 2, {1,0,0,1}));
    mainWin.addDrawable("world", new WorldPlaneDrawable(1, 10, 'y'));
    mainWin.addDrawable("vol", new Slice(&volume));
    mainWin.show();
    return app.exec();
  
  /*
  std::vector<unsigned char> slice;
  for (int i=0; i<100; i++){
    slice = volume.getSlice(i, 'd');
    //opencv test stuff
    cv::Mat image = cv::Mat(100, 100, cv::DataType<unsigned char>::type, &(slice[0]));

    if ( !image.data )
    {
        std::cout << "No image data" << std::endl;
        return -1;
    }
    cv::resize(image, image, {0,0}, 5, 5);
    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
    cv::imshow("Display Image", image);

    cv::waitKey(0);
  }
    /**/
    return 0;
}
