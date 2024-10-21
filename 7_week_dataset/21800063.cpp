#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;
using namespace std::chrono;  // chrono 사용을 편리하게 하기 위해 네임스페이스 추가

bool isLaneDetected = false;
steady_clock::time_point laneDetectedTime;
Ptr<BackgroundSubtractor> bg_model = createBackgroundSubtractorMOG2();
Mat backgroundImg; 

const double WARNING_DISPLAY_TIME = 2.0;
bool carInitiallyDetected = false;

void detectLaneDeparture(Mat& frame) {
    Mat gray, blurred, edges, mask;

    cvtColor(frame, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blurred, Size(9, 9), 2); 
    threshold(blurred, mask, 160, 255, THRESH_BINARY);
    Canny(mask, edges, 50, 150);

    Rect roi(frame.cols / 4, frame.rows * 4 / 7, frame.cols / 2, frame.rows / 4);

    Mat roiEdges = edges(roi);
    Mat roiFrame = frame(roi);  

    vector<Vec4i> lines;
    HoughLinesP(roiEdges, lines, 1, CV_PI / 180, 19, 15, 7);

    bool carOnLane = false;
    steady_clock::time_point currentTime = steady_clock::now();

    int parallelLineCount = 0;

    for (size_t i = 0; i < lines.size(); i++) {
        Vec4i l1 = lines[i];

        float dx1 = l1[2] - l1[0];
        float dy1 = l1[3] - l1[1];
        float angle1 = atan2(dy1, dx1) * 180 / CV_PI;


		if (abs(angle1 - 90) <= 14 && abs(angle1 - 90) >= 8) {
			carOnLane = true;
			laneDetectedTime = currentTime;
		}


        for (size_t j = i + 1; j < lines.size(); j++) {
            Vec4i l2 = lines[j];
            float dx2 = l2[2] - l2[0];
            float dy2 = l2[3] - l2[1];
            float angle2 = atan2(dy2, dx2) * 180 / CV_PI;

            if (abs(angle1 - angle2) < 9) { 
                float distance = abs(l1[1] - l2[1]);  
                if (distance < 15) { 
                    parallelLineCount++;
                }
            }
        }
    }

    if (parallelLineCount >= 3) {
        carOnLane = false;
    }

    if (carOnLane || (isLaneDetected && duration_cast<seconds>(currentTime - laneDetectedTime).count() < WARNING_DISPLAY_TIME)) {
        putText(frame, "Lane departure!", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
    }
}

void detectCarDisappear(Mat& currentFrame, Mat& previousFrame) {
    Mat foregroundMask, diffFrame, grayDiffFrame, frameDiff;

    bg_model->apply(currentFrame, foregroundMask);
	GaussianBlur(foregroundMask, foregroundMask, Size(11, 11), 3.5, 3.5);
	threshold(foregroundMask, foregroundMask, 10, 255, THRESH_BINARY);

    bg_model->getBackgroundImage(backgroundImg);

    bool carDisappeared = false;
    bool carStationary = false;

    if (!backgroundImg.empty()) {
        Rect roi(currentFrame.cols * 2 / 5, currentFrame.rows * 3 / 4, currentFrame.cols / 5, currentFrame.rows * 1 / 4);

        Mat roiCurrentFrame = currentFrame(roi);
        Mat roiBackgroundImg = backgroundImg(roi);

        absdiff(roiCurrentFrame, roiBackgroundImg, diffFrame);
        cvtColor(diffFrame, grayDiffFrame, COLOR_BGR2GRAY);
        Scalar totalDiff = sum(grayDiffFrame);

        if (totalDiff[0] > 1e6) {
            carDisappeared = false;
			if (carInitiallyDetected) {
                carInitiallyDetected = false;
            } 
			if (!carInitiallyDetected) {
            	carInitiallyDetected = true;
            }

        } else {
            carDisappeared = true;
        }
    }

    if (!previousFrame.empty()) {
        absdiff(currentFrame, previousFrame, frameDiff);
        cvtColor(frameDiff, frameDiff, COLOR_BGR2GRAY);
        Scalar frameChange = sum(frameDiff);

        if (frameChange[0] < 1e6) {
            carStationary = true;
        } else {
            carStationary = false;
        }
    }

    if (carInitiallyDetected && carDisappeared && carStationary) {
        putText(currentFrame, "Start Moving!", Point(50, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
    }
}

int main() {
    VideoCapture cap("Project2_video.mp4");
    if (!cap.isOpened()) {
        cout << "Error opening video stream" << endl;
        return -1;
    }

	int fps = cap.get(CAP_PROP_FPS);
	int maxFramesBeforeReset = fps * 2; 

    Mat currentFrame, previousFrame;
    bool isFirstFrame = true;

    while (true) {
        cap >> currentFrame;
        if (currentFrame.empty())
            break;

        detectLaneDeparture(currentFrame);

		detectCarDisappear(currentFrame, previousFrame);


        imshow("Project2", currentFrame);

        previousFrame = currentFrame.clone();
        isFirstFrame = false;

        if (waitKey(30) == 'q') 
            break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
