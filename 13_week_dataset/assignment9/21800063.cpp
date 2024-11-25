#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap = VideoCapture(0);
    int successes = 0;

    int numBoards = 70; // Total number of corners
    int numCornersHor = 10; // Number of horizontal corners
    int numCornersVer = 7; // Number of vertical corners
    int Rect_size = 20; // Length of one side of the rectangle

    int numSquares = (numCornersHor - 1) * (numCornersVer - 1);
    Size board_sz = Size(numCornersHor, numCornersVer);

    // Container
    vector<vector<Point3f>> object_points;
    vector<vector<Point2f>> image_points;
    vector<Point2f> corners;
    vector<Point3f> obj;

    for (int i = 0; i < numCornersHor; i++) {
        for (int j = 0; j < numCornersVer; j++) {
            obj.push_back(Point3f(i * Rect_size, j * Rect_size, 0.0f));
        }
    }

    Mat img;
    Mat gray;
    cap >> img;
    cout << "Image size: " << img.size() << endl;

    while (successes < numBoards) {
        cap >> img;
        cvtColor(img, gray, COLOR_RGB2GRAY);
        if (img.empty()) break; // End of video stream
        if (waitKey(1) == 27) break; // Stop capturing by pressing ESC

        // Finds the positions of internal corners of the chessboard
        bool found = findChessboardCorners(
            gray,        // Source chessboard view
            board_sz,    // Number of inner corners per a chessboard row and column
            corners,     // Output array of detected corners
            CALIB_CB_ADAPTIVE_THRESH // Use adaptive thresholding
        );

        if (found == 1) {
            // Refines the corner locations
            cornerSubPix(
                gray, // Input single-channel, 8-bit or float image
                corners, // Initial coordinates of input corners
                Size(11, 11), // Half of side length of the search window
                Size(-1, -1), // Half of the size of the dead region
                TermCriteria(
                    TermCriteria::EPS | TermCriteria::MAX_ITER, // Type of termination criteria
                    30, // Maximum number of iterations
                    0.1 // Desired accuracy
                )
            );

            // Renders the detected chessboard corners
            drawChessboardCorners(
                img, // Destination image
                board_sz, // Number of inner corners per chessboard row and column
                corners, // Array of detected corners
                found // Pass value of findChessboardCorners
            );

            image_points.push_back(corners);
            object_points.push_back(obj);
            printf("Snap stored\n");
            successes++;
        }

        imshow("win1", img);
        imshow("win2", gray);
        waitKey(1000);
    }

    cout << "Complete!" << "\n";

    Mat intrinsic = Mat(3, 3, CV_32FC1);
    Mat distCoeffs;
    vector<Mat> rvecs;
    vector<Mat> tvecs;

    intrinsic.ptr<float>(0)[0] = 1;
    intrinsic.ptr<float>(1)[1] = 1;

    // Finds the camera intrinsic and extrinsic parameters
    calibrateCamera(
        object_points, // The outer vector contains as many elements as number of pattern views
        image_points,  // A vector of vectors of the 2D image points
        img.size(),    // Size of the image
        intrinsic,     // Intrinsic camera matrix
        distCoeffs,    // Lens distortion coefficients
        rvecs,         // Rotation specified as a 3x1 vector
        tvecs          // 3x1 translation vector
    );

    Mat imageUndistorted;

    // Print result
    cout << "=================Intrinsic Parameter=================\n";
    for (int i = 0; i < intrinsic.rows; i++) {
        for (int j = 0; j < intrinsic.cols; j++) {
            cout << intrinsic.at<double>(i, j) << "\t";
        }
        cout << endl;
    }
    cout << "====================================================\n";

    cap.release();
    waitKey(0);
    return 0;
}
