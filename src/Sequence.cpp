#include <fstream>
#include <boost/range/adaptor/map.hpp>
#include <opencv2/opencv.hpp>
#include "ssim_thread.h"
#include "Sequence.h"

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
    return solver.Solve(frames);
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
        frame.RGBToLuma();
    }

    cout << "Done" << endl << endl;
}

void Sequence::ComputeSSIM() {
    cout << "Computing SSIM between all frames..." << endl;

    vector<thread> ssim_threads;
    int nb_threads = 0;

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

            auto th = thread(ssim_thread, ref(refFrame), ref(frame));
            ssim_threads.push_back(move(th));
            nb_threads++;

            if(nb_threads >= settings.nb_threads_ssim) {
                for(thread & th : ssim_threads) {
                    if(th.joinable()) {
                        th.join();
                        nb_threads--;
                    }
                }
                ssim_threads.clear();
            }
        }

        for(thread & th : ssim_threads) {
            if(th.joinable()) {
                th.join();
                nb_threads--;
            }
        }
        ssim_threads.clear();

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
