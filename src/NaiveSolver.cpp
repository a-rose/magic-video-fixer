#include <boost/range/adaptor/map.hpp>
#include "NaiveSolver.h"

list<Frame> NaiveSolver::Solve(map<int, Frame>& frames) {
    vector<int> available_frames;

    // Create a vector of all frames that conform to a minimum mean SSIM with
    // their candidates.
    for(auto& frame : frames | boost::adaptors::map_values) {
        if(frame.GetMeanSSIM() < settings.minimum_ssim) {
            cout << "Reject frame " << frame.GetIndex() << " with mean SSIM=" << frame.GetMeanSSIM() << endl;
        } else {
            available_frames.push_back(frame.GetIndex());
        }
    }

    return BuildSequence(frames, available_frames);
}

list<Frame> NaiveSolver::BuildSequence(map<int, Frame>& frames, vector<int>& available_frames) {
    list<Frame> sequence;

    // Set the start of the sequence
    int index = settings.first_frame;

    // For each frame, add the next best match by SSIM index
    while(!available_frames.empty()) {

        auto it = find(available_frames.begin(), available_frames.end(), index);
        if(it == available_frames.end()) {
            // Go back to the start to add any remaining frames
            it = available_frames.begin();
        }
    
        Frame& frame = frames.at(index);
        sequence.push_back(frame);

        // Find the best match
        int bestIdx = frame.BestCandidate();

        // This frame is not available anymore
        // Remove it as a candidate from all other frames
        available_frames.erase(it);
        for(auto& frame : frames | boost::adaptors::map_values) {
            frame.RemoveCandidate(index);
        }

        // Iterate
        index = bestIdx;
    }

    return sequence;
}