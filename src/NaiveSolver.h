#pragma once

#include "Solver.h"

class NaiveSolver : public Solver {
public:
    NaiveSolver(Settings s)
        : Solver(s)
    {}

    list<Frame> Solve(map<int, Frame>& frames) override;

private:
    list<Frame> BuildSequence(map<int, Frame>& frames, vector<int>& available_frames);
};