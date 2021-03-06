#pragma once

#include "SolverFabricator.h"
#include "Frame.h"
#include "settings.h"

class Sequence {
private:
    Settings settings;
    map<int, Frame> frames;
    unique_ptr<Solver> solver;

    // Video properties
    double fps;
    int width, height;

public:
    Sequence(string config_file)
        : settings(loadAppSettings(config_file))
    {
        solver = CreateSolver(settings);
    }

    void Load();
    list<Frame> Solve();
    void Write(list<Frame>&);

private:
    void LoadFrames();
    void ConvertFrames();
    void ComputeSSIM();
    void LoadSSIM();
    void ListToVideo(list<Frame>& seq);
};
