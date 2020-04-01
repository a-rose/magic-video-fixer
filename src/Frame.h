#pragma once

#include <map>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

class Frame {
private:
    uint index;
    Mat image, image_yuv;

    // Candidates
    map<int, double> candidates; // frame index -> ssim
    double mean_ssim, standard_deviation;

public:
    Frame(uint index)
        : index(index)
        , mean_ssim(0.0)
        , standard_deviation(0.0)
    {}

    uint GetIndex();
    Mat& GetImage();
    Mat& GetYUVImage();
    double GetSSIM(uint candidateIdx);
    double GetMeanSSIM();
    double GetStandardDeviation();

    void AddCandidate(uint index, double ssim);
    void RemoveCandidate(uint index);
    int BestCandidate();
    vector<double> SortCandidates();

    void RGBToYUV();
    string SSIMToString();
    void BuildStats();

    // return the number of good candidates
    int FilterCandidates(double minimumSSIM, double threshold);
};
