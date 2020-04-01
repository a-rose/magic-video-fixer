#include <fstream>
#include <boost/range/adaptor/map.hpp>
#include <boost/math/distributions/normal.hpp>
#include <opencv2/opencv.hpp>
#include "ssim.h"
#include "Sequence.h"
#include "Hungarian.h"


void Sequence::Load() {
    switch(settings.mode) {
        case 0:
            LoadFrames(),
            ComputeSSIM();
            break;

        case 1:
            LoadFrames();
            [[fallthrough]];

        case 2:
            LoadSSIM();
            break;

        default:
            throw runtime_error("Invalid mode in settings");
    }
}

list<Frame> Sequence::Solve() {
    FilterCorruptedFrames();
    
    vector<vector<double>> costMatrix = GetCostMatrix();
	HungarianAlgorithm HungAlgo;
	vector<int> assignment;
	HungAlgo.Solve(costMatrix, assignment);


    // Map to vector.  We need this vector to convert the assignment
    // indexes to frame indexes below
    vector<int> framesVec;
    for(auto& frame : frames | boost::adaptors::map_values) {
        framesVec.push_back(frame.GetIndex());
    }

    map<int, int> frameAssignment = ConvertAssignment(assignment, framesVec);
    return BuildSequence(frameAssignment, framesVec);;
}

list<Frame> Sequence::Solve2() {
    vector<int> availableFrames = FilterCorruptedFrames2();
    return BuildSequence2(availableFrames);
}

void Sequence::Write(list<Frame>& sequence) {
    cout << "Sequence complete:" << endl;

    for_each(sequence.begin(), sequence.end(), [](Frame& frame) {
        cout << frame.GetIndex() << " -> ";
    });
    cout << endl << endl;

    if(settings.mode < 2) {
        ListToVideo(sequence);
    }
}

void Sequence::LoadFrames() {
    cout << "Loading input video..." << endl;
    VideoCapture cap(settings.input_file); 

    if (cap.isOpened() == false) {
        cout << "Cannot open the video file" << endl;
        exit(1);
    }

    fps = cap.get(CAP_PROP_FPS);
    width = cap.get(CAP_PROP_FRAME_WIDTH);
    height = cap.get(CAP_PROP_FRAME_HEIGHT);

    for(int idx=0; idx < settings.frames_limit; ++idx) {
        Frame f(idx);
        Mat& img = f.GetImage();
        bool bSuccess = cap.read(img);

        // Breaking the while loop at the end of the video
        if (bSuccess == false)  {
            break;
        }

        frames.emplace(idx, f);
    }
    cap.release();

    for(auto& frame : frames | boost::adaptors::map_values) {
        frame.RGBToYUV();
    }

    cout << "Done" << endl << endl;
}

void Sequence::ComputeSSIM() {
    cout << "Computing SSIM between all frames..." << endl;

    for(auto& [refIdx, refFrame] : frames) {
        for(auto& [idx, frame] : frames) {
            
            // Don't compute a frame's ssim with itself
            if(idx == refIdx) {
                continue;
            }

            // If we already computed the ssim between these
            // two frames, we can re-use the result
            double ssim = frame.GetSSIM(refIdx);
            if(ssim != 0.0) {
                refFrame.AddCandidate(idx, ssim);
                continue;
            }

            Scalar ssimScalar = getMSSIM(refFrame.GetYUVImage(), frame.GetYUVImage());
            double ssimY = ssimScalar[0];

            // Set ssim for both frames at once
            refFrame.AddCandidate(idx, ssimY);
            frame.AddCandidate(refIdx, ssimY);
        }
        refFrame.BuildStats();
    }

    // Dump SSIM to a file
    ofstream ssimFile(settings.ssim_data_file);
    ssimFile << frames.size() << endl;

    for(auto& [index, frame] : frames) {
        ssimFile << index << " " << frame.SSIMToString() << endl;
    }

    cout << "Done" << endl << endl;
}

void Sequence::LoadSSIM() {
    ifstream ssimFile((settings.ssim_data_file));
    string line;
    istringstream iss;
    int nbFrames, frameIdx, secondFrameIdx;
    double ssimY;

    cout << "Loading SSIM from backup file..." << endl;

    getline(ssimFile, line);
    iss = istringstream(line);
    iss >> nbFrames;

    while(getline(ssimFile, line)) {
        iss = istringstream(line);
        iss >> frameIdx;

        if(settings.mode == 2) {
            frames.emplace(frameIdx, Frame(frameIdx));
        }

        Frame& frame = frames.at(frameIdx);

        for(int idx=0; idx<nbFrames; ++idx) {
            iss >> secondFrameIdx >> ssimY;
            frame.AddCandidate(secondFrameIdx, ssimY);
        }
        frame.BuildStats();
    }
    cout << "Done" << endl << endl;
}

void Sequence::FilterCorruptedFrames() {
    vector<uint> rejected_frames;

    for(auto& [index, frame] : frames) {
        int nbCandidates = frame.FilterCandidates(settings.minimum_ssim, settings.detection_threshold);

        if(nbCandidates <= 0) {
            rejected_frames.push_back(index);
        }
    }

    RemoveFrames(rejected_frames);
}

vector<int> Sequence::FilterCorruptedFrames2() {
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

    return available_frames;
}

void Sequence::RemoveFrames(const vector<uint>& rejected_frames) {
    vector<uint> corruptedFrames = {10, 17, 36, 42, 46, 69, 73, 76, 78, 83, 88, 90, 100, 101};

    for(auto& index : rejected_frames) {
        Frame frame = frames.at(index);
        cout << "Reject frame " << index << " with mean SSIM=" << frame.GetMeanSSIM() << endl;
        
        // check this frame should be removed
        auto corruptedIt = find(corruptedFrames.begin(), corruptedFrames.end(), index);
        if(corruptedIt == corruptedFrames.end()) {
            throw logic_error("this frame should not be removed !");
        }

        // Remove it from the list of frames to check
        corruptedFrames.erase(corruptedIt);

        // Remove the frame and remove it as a candidate from all other frames
        for(auto& frame : frames | boost::adaptors::map_values) {
            frame.RemoveCandidate(index);
        }
        frames.erase(index);
    }

    // If corruptedFrames is not empty, we missed a corrupted frame
    for(auto& frameIdx : corruptedFrames) {
        cout << "Missed corrupted frame " << frameIdx << endl;
    }
}

vector<vector<double>> Sequence::GetCostMatrix() {
    int y=0;
    vector<vector<double>> costMatrix{frames.size()};

    for(auto frameItY = frames.begin(); frameItY != frames.end(); ++frameItY) {
        Frame& frameY = frameItY->second; 
        vector<double>& row = costMatrix[y];

        for(auto frameItX = frames.begin(); frameItX != frames.end(); ++frameItX) {
            Frame& frameX = frameItX->second;

            double ssim = frameY.GetSSIM(frameX.GetIndex());
            row.push_back(1.0 - ssim);
        }
        y++;
    }

    return costMatrix;
}

map<int, int> Sequence::ConvertAssignment(vector<int>& assignment, vector<int>& indexes) {
    // Convert raw assignment indexes to a map
    // Also convert the assignment index to frames indexes
    map<int, int> frameAssignment;

    for (uint i=0; i<assignment.size(); ++i) {
        int index = i;
        int assignedIndex = assignment[index];

        int frameIdx = indexes[index];
        int assignedFrameIdx = indexes[assignedIndex];
        frameAssignment[frameIdx] = assignedFrameIdx;
    }
    return frameAssignment;
}

list<Frame> Sequence::BuildSequence(map<int, int>& assignment, vector<int>& indexes) {
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
                cout << "Frame not found: " << index << endl;
                throw runtime_error("Frame not found");
            }
            indexes.erase(framesVecIt);

            // Next frame
            index = assignment[index];
        }

        index = *indexes.begin();
    }
    return sequence;
}

list<Frame> Sequence::BuildSequence2(vector<int> availableFrames) {
    list<Frame> sequence;

    // Set the start of the sequence
    int index = settings.first_frame;

    // For each frame, add the next best match by SSIM index
    while(!availableFrames.empty()) {

        auto it = find(availableFrames.begin(), availableFrames.end(), index);
        if(it == availableFrames.end()) {
            // Go back to the start to add any remaining frames
            it = availableFrames.begin();
        }
    
        Frame& frame = frames.at(index);
        sequence.push_back(frame);

        // Find the best match
        int bestIdx = frame.BestCandidate();

        // This frame is not available anymore
        // Remove it as a candidate from all other frames
        availableFrames.erase(it);
        for(auto& frame : frames | boost::adaptors::map_values) {
            frame.RemoveCandidate(index);
        }

        // Iterate
        index = bestIdx;
    }

    return sequence;
}

void Sequence::ListToVideo(list<Frame>& seq) {
    cout << "Writing sequence to file..." <<endl;

    int codec = VideoWriter::fourcc('m', 'p', '4', 'v');
    VideoWriter video(settings.output_file, codec, fps, Size(width, height), true); 

    if(!video.isOpened()) {
        throw runtime_error("Failed to open output file");
    }

    while(!seq.empty()) { 
        Frame& f = seq.front();
        video.write(f.GetImage());
        seq.pop_front(); 
    }

    cout << "Done" << endl << endl;
}
