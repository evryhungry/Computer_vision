#include <opencv2/opencv.hpp>
#include <iostream>

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

    if (originalFrame.empty()) {
      originalFrame = frame.clone();  // 원본 프레임 저장
    }    
    
    int key = waitKey(delay);
    switch (key){
      case 27:
        isRun = false;
        break;

      case 'r':
        nowTransform = NONE;
        frame = originalFrame.clone();
        break;

      case 'n':
        nowTransform ^= NEGATIVE;
        break;

      case 'g':
        nowTransform ^= GAMMA
        break

    }

    if (nowTransform & NEGATIVE){
      applyNegative(frame);
    }

    if (nowTransform & GAMMA){
      applyGamma(frame);
    }

    imshow("video", frame);
  }
}