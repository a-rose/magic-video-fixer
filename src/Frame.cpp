#include "Frame.h"
#include <math.h>
#include <numeric>
#include <boost/range/numeric.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/math/distributions/normal.hpp>

uint Frame::GetIndex() {
    return index;
}

Mat& Frame::GetImage() {
    return image;
}

Mat& Frame::GetImageLuma() {
    return image_luma;
}

double Frame::GetSSIM(uint candidateIdx) {
    lock_guard<mutex> lock(mx);

    auto it = candidates.find(candidateIdx);
    if(it == candidates.end()) {
        return 0.0;
    }
    return it->second;
}

double Frame::GetMeanSSIM() {
    return mean_ssim;
}
double Frame::GetStandardDeviation() {
    return standard_deviation;
}

void Frame::AddCandidate(uint index, double ssim) {
    lock_guard<mutex> lock(mx);
    candidates.emplace(index, ssim);
}

void Frame::RemoveCandidate(uint index) {
    candidates.erase(index);
}

int Frame::BestCandidate() {
    auto bestIt = max_element(candidates.begin(),candidates.end(),[] (const auto& a, const auto& b)->bool{ return a.second < b.second; } );
    return bestIt->first;
}

vector<double> Frame::SortCandidates() {
    vector<double> candidates2;
    boost::range::copy(candidates | boost::adaptors::map_values, back_inserter(candidates2));
   
    // Reverse sort (descending SSIM)
    sort(candidates2.rbegin(), candidates2.rend());
    return candidates2;
}

void Frame::RGBToLuma() {
    Mat image_yuv, yuv[3];
  
    // Convert to YUV, then split YUV channels in 3 separate Mats
    cvtColor(image, image_yuv, COLOR_BGR2YUV);
    split(image_yuv, yuv);
    image_luma = yuv[0];
}

string Frame::SSIMToString() {
    stringstream str;
    for(auto& [index, ssim] : candidates) {
        str << index << " " << ssim << " ";
    }
    return str.str();
}

void Frame::BuildStats() {
    int size = candidates.size();

    // Compute the mean SSIM for this frame
    const auto sum = boost::accumulate(candidates | boost::adaptors::map_values, 0.0);
    mean_ssim = sum / size;


    // Compute the standard deviation
    double t = 0;
    for(auto& ssim : candidates | boost::adaptors::map_values) {
        t += (ssim - mean_ssim) * (ssim - mean_ssim);
    }

    t/= size;
    standard_deviation = sqrt(t);
}

// return the number of good candidates
int Frame::FilterCandidates(double minimumSSIM, double threshold) {

    if(mean_ssim < minimumSSIM) {
        return 0;
    }

    boost::math::normal norm(mean_ssim, standard_deviation);
    double threshold_high = boost::math::quantile(norm, threshold);
    double threshold_low = boost::math::quantile(norm, 1-threshold);

    for(auto it=candidates.begin(); it != candidates.end();) {
        double p = it->second;

        if(p > threshold_high || p < threshold_low) {
            candidates.erase(it++);
        } else {
            ++it;
        }
    }

    return candidates.size();
}
