#include <boost/range/adaptor/map.hpp>
#include "ChallengeSolver.h"
#include "Hungarian.h"

list<Frame> ChallengeSolver::Solve(map<int, Frame>& frames) {
    FilterCorruptedFrames(frames);
    
    vector<vector<double>> cost_matrix = GetCostMatrix(frames);
	HungarianAlgorithm hung_algo;
	vector<int> assignment;
	hung_algo.Solve(cost_matrix, assignment);


    // Map to vector.  We need this vector to convert the assignment
    // indexes to frame indexes below
    vector<int> frames_indexes;
    for(auto& frame : frames | boost::adaptors::map_values) {
        frames_indexes.push_back(frame.GetIndex());
    }

    map<int, int> frame_assignment = ConvertAssignment(assignment, frames_indexes);
    return BuildSequence(frames, frame_assignment, frames_indexes);
}

void ChallengeSolver::FilterCorruptedFrames(map<int, Frame>& frames) {
    vector<uint> rejected_frames;

    for(auto& [index, frame] : frames) {
        int nbCandidates = frame.FilterCandidates(settings.minimum_ssim, settings.detection_threshold);

        if(nbCandidates <= 0) {
            rejected_frames.push_back(index);
        }
    }

    RemoveFrames(frames, rejected_frames);
}

list<Frame> ChallengeSolver::BuildSequence(map<int, Frame>& frames, map<int, int>& assignment, vector<int>& indexes) {
    list<Frame> sequence;
    vector<int> tested; // frames already added to the sequence
    int index = settings.first_frame;

    while(!indexes.empty()) {
        while(true) {
            // If we already checked this index, it means we are going in a loop
            auto it = find(tested.begin(), tested.end(), index);
            if(it != tested.end()) {
                break;
            }

            tested.push_back(index);
            sequence.push_back(frames.at(index));

            // We don't need this frame anymore
            auto framesVecIt = find(indexes.begin(), indexes.end(), index);
            if(framesVecIt == indexes.end()) {
                throw runtime_error("Frame not found:" + to_string(index));
            }
            indexes.erase(framesVecIt);

            // Next frame
            index = assignment[index];
        }

        index = *indexes.begin();
    }
    return sequence;
}


void ChallengeSolver::RemoveFrames(map<int, Frame>& frames, const vector<uint>& rejected_frames) {
    vector<uint> corrupted_frames = {10, 17, 36, 42, 46, 69, 73, 76, 78, 83, 88, 90, 100, 101};

    for(auto& index : rejected_frames) {
        Frame frame = frames.at(index);
        cout << "Reject frame " << index << " with mean SSIM=" << frame.GetMeanSSIM() << endl;
        
        // check this frame should be removed
        auto corruptedIt = find(corrupted_frames.begin(), corrupted_frames.end(), index);
        if(corruptedIt == corrupted_frames.end()) {
            throw logic_error("this frame should not be removed !");
        }

        // Remove it from the list of frames to check
        corrupted_frames.erase(corruptedIt);

        // Remove the frame and remove it as a candidate from all other frames
        for(auto& frame : frames | boost::adaptors::map_values) {
            frame.RemoveCandidate(index);
        }
        frames.erase(index);
    }

    // If corrupted_frames is not empty, we missed a corrupted frame
    for(auto& frameIdx : corrupted_frames) {
        cout << "Missed corrupted frame " << frameIdx << endl;
    }
}

vector<vector<double>> ChallengeSolver::GetCostMatrix(map<int, Frame>& frames) {
    int y=0;
    vector<vector<double>> cost_matrix{frames.size()};

    for(auto& frame_y : frames | boost::adaptors::map_values) {
        vector<double>& row = cost_matrix[y];

        for(auto& index_x : frames | boost::adaptors::map_keys) {
            double ssim = frame_y.GetSSIM(index_x);
            row.push_back(1.0 - ssim);
        }
        y++;
    }

    return cost_matrix;
}

map<int, int> ChallengeSolver::ConvertAssignment(const vector<int>& assignment, vector<int>& indexes) {
    // Convert raw assignment indexes to a map
    // Also convert the assignment index to frames indexes
    map<int, int> frame_assignment;

    for (uint index=0; index<assignment.size(); ++index) {
        int assignedIndex = assignment[index];

        int frameIdx = indexes[index];
        int assignedFrameIdx = indexes[assignedIndex];
        frame_assignment[frameIdx] = assignedFrameIdx;
    }
    return frame_assignment;
}
