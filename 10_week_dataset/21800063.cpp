
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace std;
using namespace cv;

// Global Variables
Mat frame;
CascadeClassifier face_classifier;
bool is_tracking = false;
char last_detected = ' ';
int min_size = 0, max_size = 0; 
vector<Rect> faces; 

vector<Rect> n_detectFaces(Mat &frame, int min_size, int max_size) {
    vector<Rect> detected_faces;
    face_classifier.detectMultiScale(frame, detected_faces, 1.1, 3, 0, Size(min_size, min_size), Size(max_size, max_size));
    return detected_faces;
}

vector<Rect> m_detectFaces(Mat &frame, int min_size, int max_size) {
    vector<Rect> detected_faces;
    face_classifier.detectMultiScale(frame, detected_faces, 1.1, 9, 0, Size(min_size, min_size), Size(max_size, max_size));
    return detected_faces;
}

vector<Rect> f_detectFaces(Mat &frame, int min_size, int max_size) {
    vector<Rect> detected_faces;
    face_classifier.detectMultiScale(frame, detected_faces, 1.09, 4, 0, Size(min_size, min_size), Size(max_size, max_size));
    return detected_faces;
}

void updateFaceTracking() {
    if (last_detected == ' ') return;
    else if (last_detected == 'f') faces = f_detectFaces(frame, min_size, max_size);
    else if (last_detected == 'n') faces = n_detectFaces(frame, min_size, max_size);
    else if (last_detected == 'm') faces = m_detectFaces(frame, min_size, max_size);

    for (size_t i = 0; i < faces.size(); i++) {
        Point lb(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
        Point tr(faces[i].x, faces[i].y);
        rectangle(frame, lb, tr, Scalar(0, 255, 0), 3, 4, 0); // Draw a rectangle around each face
    }
}

void trackFace() {
    if (is_tracking && last_detected == ' ') {
        putText(frame, "Detect before tracking", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
    }

    if (is_tracking && last_detected != ' ') {
        Mat tracking_window = Mat::zeros(frame.size(), frame.type());
        tracking_window = Scalar(255, 0, 0);

        for (size_t i = 0; i < faces.size(); i++) {
            Mat face_roi = frame(faces[i]);
            face_roi.copyTo(tracking_window(faces[i]));
        }

        imshow("tracking", tracking_window);
    } else {
        destroyWindow("tracking");
    }
}

int main() {
    if (!face_classifier.load("haarcascade_frontalface_default.xml")) {
        cerr << "Error loading face cascade" << endl;
        return -1;
    }

    VideoCapture cap("Faces.mp4");
    if (!cap.isOpened()) {
        cerr << "Error opening video stream" << endl;
        return -1;
    }

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        if (last_detected != ' ') {
            updateFaceTracking(); 
        }

        trackFace();

        imshow("Faces", frame);

        char key = (char)waitKey(30);
        if (key == 'q') break;

        switch (key) {
            case 'n':
                min_size = 88;
                max_size = 89;
                last_detected = 'n';
                break;
            case 'm':
                min_size = 59;
                max_size = 60;
                last_detected = 'm';
                break;
            case 'f':
                min_size = 35;
                max_size = 43;
                last_detected = 'f';
                break;
            case 'r':
                last_detected = ' ';
                is_tracking = false;
                destroyWindow("tracking");
                break;
            case 't':
                is_tracking = !is_tracking;
                break;
            default:
                break;
        }
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
