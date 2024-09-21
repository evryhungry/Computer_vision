#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;

enum TransformType {
  NONE       = 0,
  NEGATIVE   = 1 << 0,
  GAMMA      = 1 << 1, 
  HISTOGRAM  = 1 << 2, 
  SLICING    = 1 << 3, 
  CONVERSION = 1 << 4,  
  AVERAGE    = 1 << 5,  
  SHARPENING = 1 << 6, 
  WHITE      = 1 << 7,   
};

void applyNegative(Mat& frame) {
  frame = 255 - frame;
}

void applyGamma(Mat& frame){
  float gamma = 2.5;
  unsigned char pix[256];

  for (int i = 0; i < 256 ; i++){
    pix[i] = saturate_cast<uchar>(pow((float)(i / 255.0), gamma) * 255.0f);
  }

  for (int y = 0; y < frame.rows; y++){
    for (int x = 0; x < frame.cols; x++) {

      Vec3b& pixel = frame.at<Vec3b>(y, x);
      pixel[0] = pix[pixel[0]];
      pixel[1] = pix[pixel[1]];
      pixel[2] = pix[pixel[2]];
    }
  }
}

void applyHistogram(Mat& frame){
  Mat gray;
  cvtColor(frame, gray, COLOR_BGR2GRAY);

  Mat equalized;
  equalizeHist(gray, equalized);

  cvtColor(equalized, frame, COLOR_GRAY2BGR);
}


void applySlicing(Mat& frame){
  vector<Mat> mo(3);
  Mat HSV;

  uchar* h;
  uchar* s;

  cvtColor(frame, HSV, COLOR_BGR2HSV);
  split(HSV, mo);

  for (int y = 0; y < frame.rows; y++){
    h = mo[0].ptr<uchar>(y);
    s = mo[1].ptr<uchar>(y);
    
    for (int x = 0; x < frame.cols; x++) {
      if (h[x] > 9 && h[x] < 23) s[x] = s[x];
      else s[x] = 0;
    }
  }

  merge(mo, HSV);
  cvtColor(HSV, frame, COLOR_HSV2BGR);
}

void applyConversion(Mat& frame){
  vector<Mat> cc(3);
  Mat HSV;

  uchar* h;
  uchar* s;

  cvtColor(frame, HSV, COLOR_BGR2HSV);
  split(HSV, cc);

  for (int y = 0; y < frame.rows; y++){
    h = cc[0].ptr<uchar>(y);
    s = cc[1].ptr<uchar>(y);
    
    for (int x = 0; x < frame.cols; x++) {
      if (h[x] + 50 > 179) h[x] = h[x] + 50 - 179; 
      else h[x] += 50;
    }
  }

  merge(cc, HSV);
  cvtColor(HSV, frame, COLOR_HSV2BGR);
}


int main(){
  Mat frame, originalFrame;
  VideoCapture cap("video.mp4");
  int fps, delay;

  fps = cap.get(CAP_PROP_FPS);
  delay = 1000 / fps;
  
  bool isRun = true;
  int nowTransform = NONE;

  
  while (isRun){
    cap >> frame;
    
    int key = waitKey(delay);
    switch (key){
      case 27:
        isRun = false;
        break;
      
      case 'r':
        nowTransform = NONE;
        break;

      case 'n':
        nowTransform = NEGATIVE;
        break;

      case 'g':
        nowTransform = GAMMA;
        break;

      case 'h':
        nowTransform = HISTOGRAM;
        break;

      case 's':
        nowTransform = SLICING;
        break;

      case 'c':
        nowTransform = CONVERSION;
        break;       
    }

    if (nowTransform & NEGATIVE) {
        applyNegative(frame);
    }

    if (nowTransform & GAMMA) {
        applyGamma(frame); 
    }

    if (nowTransform & HISTOGRAM) {
        applyHistogram(frame); 
    }

    if (nowTransform & SLICING) {
        applySlicing(frame);  
    }

    if (nowTransform & CONVERSION) {
        applyConversion(frame);  
    }
    
    imshow("video", frame);
  }
}