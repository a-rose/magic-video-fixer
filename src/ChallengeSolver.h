#pragma once

#include "Solver.h"

class ChallengeSolver : public Solver {
public:
    ChallengeSolver(Settings s)
        : Solver(s)
    {}

    list<Frame> Solve(map<int, Frame>& frames) override;

private:
    void FilterCorruptedFrames(map<int, Frame>& frames);
    list<Frame> BuildSequence(map<int, Frame>& frames, map<int, int>& assignment, vector<int>& indexes);

    void RemoveFrames(map<int, Frame>& frames, const vector<uint>& rejected_frames);
    vector<vector<double>> GetCostMatrix(map<int, Frame>& frames);
    map<int, int> ConvertAssignment(const vector<int>& assignment, vector<int>& indexes);
};