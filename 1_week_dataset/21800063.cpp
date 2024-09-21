#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
  Mat frame;
  int fps;
  int delay;
  int total_fps;
  VideoCapture cap;

  if (cap.open("background.mp4") == 0) {
    cout << "no such file!" << endl;
    waitKey(0); 
  }

  total_fps = cap.get(CAP_PROP_FRAME_COUNT);
  fps = cap.get(CAP_PROP_FPS);
  delay = 1000 / fps;

  int frames_to_run = fps * 3;

  while (1) {
    cap >> frame;
    if (frame.empty()) {
    cout << "end of video" << endl;
    break;
  }

    int current_fps = cap.get(CAP_PROP_POS_FRAMES);
    cout << "frames: " << current_fps << " / " << total_fps << "\n";

    if (current_fps >= frames_to_run) break;

    imshow("video", frame);
    waitKey(delay);
  }
}


