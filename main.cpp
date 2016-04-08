#include <iostream>
#include <opencv2/opencv.hpp>

#include "volume.hpp"

#include <QApplication>
#include "viewer/viewWindow.hpp"
#include "viewer/drawables/shape.hpp"
#include "viewer/drawables/slice.hpp"
#include "viewer/drawables/mesh.hpp"


int main(int argc, char** argv )
{
  /*Volume<unsigned char> volume;//Volume<char>();
  if ( argc != 2 )
  {
    std::cout << "usage: " << argv[0] << " dataFile.3d" << std::endl;
    // default, create a sphere like object at the center with decreasing intensity value
    volume = Volume<unsigned char>({100, 50, 25});
    for (int i=0;i<100;i++) for (int j=0;j<50;j++) for (int k=0;k<25;k++)
    {
      //float dist = sqrt((i-50)*(i-50) + (j-50)*(j-50) + (k-50)*(k-50));
      //volume(i,j,k) = 255*pow((100-dist)/100,4);
    }
    for(int i=0; i<25; i++) volume(i,i,i)=255;
    //volume.toFile_bin("vol.dat");
  } else {
    volume.fromFile_bin(std::string(argv[1]));
  }
  //Volume<unsigned char> vol2 = Volume<unsigned char>({1000,500,250}, 128);
  //vol2.toFile("vol_perlin.txt");
  //std::cout << volume.getShape() << std::endl;
  */
#if true
    QApplication app(argc, argv);
    ViewerWindow mainWin;
    //mainWin.addDrawable("cube1", new CubeDrawable({0,0,0}, 5, {0,1,0,1}));
    //mainWin.addDrawable("cube1", new CubeDrawable({-2.5,-2.5,-2.5}, 2, {1,0,0,1}));
    //mainWin.addDrawable("world", new WorldPlaneDrawable(1, 10, 'y'));
    //mainWin.addDrawable("vol", new SliceDrawable(&volume));
    mainWin.addDrawable("mesh", new MeshDrawable("/home/letouzea/downloads/4drep/antoine_lucie1/Models/Off/model_0000010000.off"));
    /*std::vector<std::string> filenames;
    for (int i=1; i<argc; i++) filenames.push_back(std::string(argv[i]));
    mainWin.addDrawable("mesh", new MeshSeqDrawable(filenames));
    */
    mainWin.show();
    return app.exec();
#else 
  //*
  for (int i=0; i<100; i++){
    auto slice = volume.getSlice(i, 'h');
    //opencv test stuff
    cv::Mat image = cv::Mat(50, 25, cv::DataType<unsigned char>::type, slice.data.data());

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
#endif
    return 0;
}
