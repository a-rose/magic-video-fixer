#include "SolverFabricator.h"

unique_ptr<Solver> CreateSolver(Settings settings) {
    if(settings.algo == 0) {
        return make_unique<ChallengeSolver>(settings);
    } else {
        return make_unique<NaiveSolver>(settings);
    }
}
