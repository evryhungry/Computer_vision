#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cv; 
using namespace std;

int main() {
    Mat query_image, compare_img, descriptors_query, descriptors_compare, imgMatches;
    Ptr<ORB> orbF = ORB::create(1000);
    vector<KeyPoint> keypoints_query, keypoints_compare;
    vector<vector<DMatch>> matches;
    vector<DMatch> goodMatches;
    BFMatcher matcher(NORM_HAMMING);

    string query_name;
    cout << "Enter query image name: ";
    cin >> query_name;

    query_image = imread(query_name);
    if (query_image.empty()) { 
        cout << "No file found!" << endl; 
        return -1;
    }
    resize(query_image, query_image, Size(640, 480));

    String path("Handong*_1.jpg");
    vector<String> db_image_paths;
    glob(path, db_image_paths, false);

    if (db_image_paths.empty()) {
        cout << "Image database is empty or images do not exist." << endl;
        return -1;
    }

    int maxMatchSize = 0;
    string bestMatchPath;

    for (const auto& db_image_path : db_image_paths) {
        compare_img = imread(db_image_path);
        if (compare_img.empty()) continue;

        resize(compare_img, compare_img, Size(640, 480));
        
        orbF->detectAndCompute(query_image, noArray(), keypoints_query, descriptors_query);
        orbF->detectAndCompute(compare_img, noArray(), keypoints_compare, descriptors_compare);

        matches.clear();
        matcher.knnMatch(descriptors_compare, descriptors_query, matches, 2);

        vector<DMatch> currentGoodMatches;
        for (int i = 0 ; i < matches.size() ; i++ ) {
            if (matches.at(i).size() == 2 && matches.at(i).at(0).distance <= 0.6f * matches.at(i).at(1).distance) {
                currentGoodMatches.push_back(matches[i][0]);
            }
        }

        if (currentGoodMatches.size() > maxMatchSize) {
            maxMatchSize = currentGoodMatches.size();
            bestMatchPath = db_image_path;
            goodMatches = currentGoodMatches;
        }
    }

    if (!bestMatchPath.empty()) {
        compare_img = imread(bestMatchPath);
        resize(compare_img, compare_img, Size(640, 480));

        drawMatches(compare_img, keypoints_compare, query_image, keypoints_query, goodMatches, imgMatches,
                    Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

        imshow("Query Image", query_image);
        imshow("Best Matching Image", imgMatches);
        waitKey(0);
    }

    return 0;
}