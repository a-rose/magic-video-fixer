#pragma once

#include <memory>
#include "ChallengeSolver.h"
#include "NaiveSolver.h"

using namespace std;

unique_ptr<Solver> CreateSolver(Settings settings);