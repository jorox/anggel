#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <list>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#define _USE_MATH_DEFINES
#include <cmath>

using namespace cv;

struct MyMat
{
  Mat _mat;
  std::string _fname;
  std::list<Point> _points;
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
  std::cout << "curr = " << *(info->shift)-1 << std::endl;
  imshow( "imgDisplay", it->_mat );
}

double angle(Point& x1, Point& x2){
  Point2f xi = x2 - x1;
  Point2f e1 (1.0,0.0);
  if (xi.dot(e1) < 0){ xi = x1 - x2;}
  // Normalize
  xi *=  1.0 / sqrt( xi.dot(xi) );
  double res = acos ( xi.dot(e1) ) / M_PI * 180.;
  std::cout << res << std::endl;
  return res;
}

void onmouse(int event, int x, int y, int flags, void* data)
{
  if(event==CV_EVENT_LBUTTONDOWN)
    {
      std::cout << "Click= " << x << ", " << y << std::endl;
      Slider * info = (struct Slider*) data;
      std::list<MyMat>::iterator it = *(info->start);
      std::advance (it, *(info->shift));
      Point newPoint (x, y);

      circle (it->_mat, newPoint, 5, Scalar(0,255,0), -5); // draw a circle

      if (it->_points.size() % 2 != 0){
        double ang = angle ( it->_points.front(), newPoint );
        putText ( it->_mat, std::to_string(ang), it->_points.front() + Point(0, 20),
                  1, 1, Scalar(0,255,0), 2);
      }

      it->_points.push_front(newPoint); // add point to image
      imshow ("imgDisplay", it->_mat);
    }
}

double calc_angle(double y, double yp, double x, double xp){
  return atan ( (y - yp) / (x - xp) ) / 3.1415 * 180.;
}

/**

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
**/
int main (int argc, char** argv)
{

  int numOfImages = argc - 1;
  std::cout << "... Number of files = " << numOfImages << std::endl;
  std::list<MyMat> images;
  std::list<MyMat>::iterator it_first_image;
  int curr_image = 0;
  Slider sld;

  for (int i = 0; i < numOfImages; ++i){
    std::cout << "loading " << argv[i+1] << std::endl;
    MyMat tmp;
    tmp._mat = imread (argv[i+1]);
    tmp._fname = argv[i+1];
    images.push_back( tmp );
  }
  it_first_image = images.begin();
  sld.shift = &curr_image;
  sld.start = &it_first_image;

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
  imshow( "imgDisplay", images.begin()->_mat );

  waitKey();
  //write_results (images);
  return 0;

}
