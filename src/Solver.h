#pragma once

#include "ChallengeSolver.h"
#include "NaiveSolver.h"

class Solver {
protected:
    Settings settings;
    SolverImplem* implem;

public:
    Solver(Settings s) 
        : settings(s)
    {
        if(s.algo == 0) {
            implem = new ChallengeSolver(s);
        } else {
            implem = new NaiveSolver(s);
        }
    }

    ~Solver() {
        if(settings.algo == 0) {
            ChallengeSolver* s = (ChallengeSolver*) implem;
            delete s;
        } else {
            NaiveSolver* s = (NaiveSolver*) implem;
            delete s;
        }
    }

    list<Frame> Solve(map<int, Frame>& frames) {
        return implem->Solve(frames);
    }

};