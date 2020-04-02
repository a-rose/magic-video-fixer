#pragma once

#include <map>
#include <mutex>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

class Frame {
private:
    uint index;
    Mat image, image_luma;
    mutex mx;

    // Candidates
    map<int, double> candidates; // frame index -> ssim
    double mean_ssim, standard_deviation;

public:
    Frame(uint index)
        : index(index)
        , mean_ssim(0.0)
        , standard_deviation(0.0)
    {}

    // Copy constructor
    // TODO should lock f.mx before copying but f must be const
    Frame(const Frame& f)
        : index(f.index)
        , image(f.image)
        , image_luma(f.image_luma)
        , candidates(f.candidates)
        , mean_ssim(f.mean_ssim)
        , standard_deviation(f.standard_deviation)
    {}
    
    uint GetIndex();
    Mat& GetImage();
    Mat& GetImageLuma();
    double GetSSIM(uint candidateIdx);
    double GetMeanSSIM();
    double GetStandardDeviation();

    void AddCandidate(uint index, double ssim);
    void RemoveCandidate(uint index);
    int BestCandidate();
    vector<double> SortCandidates();

    void RGBToLuma();
    string SSIMToString();
    void BuildStats();

    // return the number of good candidates
    int FilterCandidates(double minimumSSIM, double threshold);
};
