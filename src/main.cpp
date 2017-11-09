#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <list>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace cv;

struct MyMat
{
  Mat _mat;
  std::string _fname;
  double _pointx[6] = { 0., 0., 0., 0., 0., 0.};
  double _pointy[6] = { 0., 0., 0., 0., 0., 0.};
  int active_point = 0;
};

struct Slider
{
  int* shift;
  const std::list<MyMat>::iterator* start;
};

void on_trackbar( int , void* data)
{
  Slider * info = (struct Slider*) data;
  std::list<MyMat>::iterator it = *(info->start);
  std::advance(it, *(info->shift) );
  std::cout << "curr = " << *(info->shift) << std::endl;
  imshow( "imgDisplay", it->_mat );
}

void onmouse(int event, int x, int y, int flags, void* data)
{
  if(event==CV_EVENT_LBUTTONDOWN)
    {
      std::cout << "Click= " << x << ", " << y << std::endl;
      Slider * info = (struct Slider*) data;
      std::list<MyMat>::iterator it = *(info->start);
      std::advance (it, *(info->shift));

      circle (it->_mat, Point(x,y), 5, Scalar(0,255,0), -5);
      it->_pointx[it->active_point] = x;
      it->_pointy[it->active_point] = y;
      it->active_point += 1;
      it->active_point %= 6;
      imshow ("imgDisplay", it->_mat);
    }
}

double calc_angle(double y, double yp, double x, double xp){
  return atan ( (y - yp) / (x - xp) ) / 3.1415 * 180.;
}


void write_results( std::list<MyMat>& imgList)
{
  std::ofstream ofs;
  ofs.open ("results.txt", std::ofstream::out);
  ofs << "#filename theta1 theta2 theta3 points";

  for (std::list<MyMat>::iterator it = imgList.begin();
       it != imgList.end(); ++it){

    //write new filename
    ofs << std::endl << it->_fname;

    //calculate the three angles
    double theta1 = calc_angle ( it->_pointy[1], it->_pointy[0],
                                 it->_pointx[1], it->_pointx[0] );
    double theta2 = calc_angle ( it->_pointy[2], it->_pointy[3],
                                 it->_pointx[3], it->_pointx[2] );
    double theta3 = calc_angle ( it->_pointy[4], it->_pointy[5],
                                 it->_pointx[5], it->_pointx[4] );

    // write the angles
    ofs << " " << theta1 << " " << theta2 << " " << theta3;

    // write points
    for (int i = 0; i<6; ++i){
      ofs << " (" << it->_pointx[i] << "," << it->_pointy[i] <<")";
    }
  }

  ofs.close();
  std::cout << "... done writing to results.txt" << std::endl;
}

int main (int argc, char** argv)
{

  int numOfImages = argc - 1;
  std::cout << "... Number of files = " << numOfImages << std::endl;
  std::list<MyMat> images;
  std::list<MyMat>::iterator it_curr_image;
  int curr_image = 0;
  Slider sld;
  sld.shift = &curr_image;
  sld.start = &it_curr_image;

  for (int i = 0; i < numOfImages; ++i){
    std::cout << "loading " << argv[i+1] << std::endl;
    MyMat tmp;
    tmp._mat = imread (argv[i+1]);
    tmp._fname = argv[i+1];
    images.push_back( tmp );
  }
  it_curr_image = images.begin();

  std::cout << "... loaded " << images.size() << " images into memory"
            << std::endl;

  // Create the trackbar
  char trackbarName[50];
  sprintf( trackbarName, "Image x %d", numOfImages );

  // Create the window
  namedWindow("imgDisplay", 1);
  setMouseCallback("imgDisplay", onmouse,
                   &sld );


  createTrackbar( trackbarName, "imgDisplay", &curr_image, numOfImages-1,
                  on_trackbar, &sld);

  // show first image
  imshow( "imgDisplay", it_curr_image->_mat );

  waitKey();
  write_results (images);
  return 0;

}
