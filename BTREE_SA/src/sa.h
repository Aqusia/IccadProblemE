#ifndef SA_H
#define SA_H

/** Simulated annealing stages used by the solver. */
enum class SAStage
{
    BROAD_EXPLORATION,
    LEGALIZATION,
    REFINEMENT,
    DIVERSIFICATION
};

/** StageConfig describes one temperature schedule and move policy block. */
struct StageConfig
{
    StageConfig()
        : stage(SAStage::BROAD_EXPLORATION), tauBegin(0.0), tauEnd(0.0),
          overflowWeightBegin(0.0), overflowWeightEnd(0.0), initialAcceptProb(0.85),
          coolingRate(0.95), movesPerTempFactor(10), noImproveLimit(6),
          maxTemperatureSteps(20), lockFeasible(false), kickMoves(0)
    {
    }

    StageConfig(SAStage stage, double tauBegin, double tauEnd,
                double overflowWeightBegin, double overflowWeightEnd,
                double initialAcceptProb, double coolingRate,
                int movesPerTempFactor, int noImproveLimit, int maxTemperatureSteps,
                bool lockFeasible, int kickMoves)
        : stage(stage), tauBegin(tauBegin), tauEnd(tauEnd),
          overflowWeightBegin(overflowWeightBegin), overflowWeightEnd(overflowWeightEnd),
          initialAcceptProb(initialAcceptProb), coolingRate(coolingRate),
          movesPerTempFactor(movesPerTempFactor), noImproveLimit(noImproveLimit),
          maxTemperatureSteps(maxTemperatureSteps), lockFeasible(lockFeasible),
          kickMoves(kickMoves)
    {
    }

    SAStage stage;
    double tauBegin;
    double tauEnd;
    double overflowWeightBegin;
    double overflowWeightEnd;
    double initialAcceptProb;
    double coolingRate;
    int movesPerTempFactor;
    int noImproveLimit;
    int maxTemperatureSteps;
    bool lockFeasible;
    int kickMoves;
};

/** NormStats stores robust normalizers for area and wire terms. */
struct NormStats
{
    NormStats() : areaNorm(1.0), wireNorm(1.0) {}
    NormStats(double areaNorm, double wireNorm) : areaNorm(areaNorm), wireNorm(wireNorm) {}

    double areaNorm;
    double wireNorm;
};

#endif
