#include "floorplanner.h"

#include <algorithm>
#include <cmath>

using namespace std;

/** Return the median of a sample vector. */
static double medianOf(vector<double> values)
{
    if (values.empty())
    {
        return 1.0;
    }
    sort(values.begin(), values.end());
    const size_t mid = values.size() / 2;
    if (values.size() % 2 == 1)
    {
        return values[mid];
    }
    return 0.5 * (values[mid - 1] + values[mid]);
}

/** Compare two feasible states using the final output preference order. */
bool Floorplanner::betterFeasibleForOutput(const SolutionState &lhs,
                                           const SolutionState &rhs) const
{
    if (!lhs.valid)
    {
        return false;
    }
    if (!rhs.valid)
    {
        return true;
    }
    if (_gaussianStats.valid)
    {
        const double lhsScore = calcGaussianFeasibleScore(lhs, _gaussianStats);
        const double rhsScore = calcGaussianFeasibleScore(rhs, _gaussianStats);
        if (std::fabs(lhsScore - rhsScore) > 1e-12)
        {
            return lhsScore < rhsScore;
        }
    }
    if (std::fabs(lhs.reportCost - rhs.reportCost) > 1e-12)
    {
        return lhs.reportCost < rhs.reportCost;
    }
    if (lhs.area != rhs.area)
    {
        return lhs.area < rhs.area;
    }
    if (std::fabs(lhs.wireLength - rhs.wireLength) > 1e-12)
    {
        return lhs.wireLength < rhs.wireLength;
    }
    return lhs.searchCost < rhs.searchCost;
}

/** Convert raw overflow into a normalized ratio against the outline. */
double Floorplanner::calcOverflowRatio(size_t overflowWidth, size_t overflowHeight) const
{
    const double overflowX = static_cast<double>(overflowWidth) / max(static_cast<double>(_outlineWidth), 1.0);
    const double overflowY = static_cast<double>(overflowHeight) / max(static_cast<double>(_outlineHeight), 1.0);
    return overflowX + overflowY;
}

/** Snapshot the current solver state so it can be restored later. */
Floorplanner::SolutionState Floorplanner::captureSolution() const
{
    SolutionState snapshot;
    snapshot.nodes = _nodes;
    snapshot.rotations.reserve(_blkList.size());
    snapshot.blockPositions.reserve(_blkList.size());
    snapshot.rootIdx = _rootIdx;

    for (const Block *blk : _blkList)
    {
        snapshot.rotations.push_back(blk->isRotated());
        snapshot.blockPositions.push_back({blk->getX1(), blk->getY1(), blk->getX2(), blk->getY2()});
    }

    snapshot.packedWidth = _packedWidth;
    snapshot.packedHeight = _packedHeight;
    snapshot.area = _area;
    snapshot.overflowWidth = _overflowWidth;
    snapshot.overflowHeight = _overflowHeight;
    snapshot.wireLength = _wireLength;
    snapshot.reportCost = _reportCost;
    snapshot.searchCost = _searchCost;
    snapshot.feasible = isFeasible();
    snapshot.valid = true;
    return snapshot;
}

/** Restore a previously captured solution snapshot. */
void Floorplanner::restoreSolution(const SolutionState &solution)
{
    if (!solution.valid)
    {
        return;
    }

    _nodes = solution.nodes;
    _rootIdx = solution.rootIdx;
    for (size_t i = 0; i < _blkList.size(); ++i)
    {
        _blkList[i]->setRotate(solution.rotations[i]);
        const array<size_t, 4> &pos = solution.blockPositions[i];
        _blkList[i]->setPos(pos[0], pos[1], pos[2], pos[3]);
    }

    _packedWidth = solution.packedWidth;
    _packedHeight = solution.packedHeight;
    _area = solution.area;
    _overflowWidth = solution.overflowWidth;
    _overflowHeight = solution.overflowHeight;
    _wireLength = solution.wireLength;
    _reportCost = solution.reportCost;
    _searchCost = solution.searchCost;
    rebuildNameToNodeIdx();
}

/** Estimate robust area and wire normalizers from a pool of states. */
NormStats Floorplanner::computeNormFromPool(const vector<SolutionState> &pool, double tauNorm) const
{
    vector<double> areaSamples;
    vector<double> wireSamples;
    vector<pair<double, size_t>> ranked;
    ranked.reserve(pool.size());

    for (size_t i = 0; i < pool.size(); ++i)
    {
        if (!pool[i].valid)
        {
            continue;
        }

        const double overflow = calcOverflowRatio(pool[i].overflowWidth, pool[i].overflowHeight);
        ranked.push_back({overflow, i});
        if (overflow <= tauNorm + 1e-12)
        {
            areaSamples.push_back(static_cast<double>(max(pool[i].area, _totalBlockArea)));
            wireSamples.push_back(max(pool[i].wireLength, 1.0));
        }
    }

    if (areaSamples.size() < min<size_t>(4, pool.size()))
    {
        sort(ranked.begin(), ranked.end());
        const size_t fallbackCount = min<size_t>(max<size_t>(4, ranked.size() / 2), ranked.size());
        areaSamples.clear();
        wireSamples.clear();
        for (size_t i = 0; i < fallbackCount; ++i)
        {
            const SolutionState &state = pool[ranked[i].second];
            areaSamples.push_back(static_cast<double>(max(state.area, _totalBlockArea)));
            wireSamples.push_back(max(state.wireLength, 1.0));
        }
    }

    NormStats stats;
    stats.areaNorm = max(medianOf(areaSamples), 1.0);
    stats.wireNorm = max(medianOf(wireSamples), 1.0);
    return stats;
}

/** Fit a simple Gaussian model over feasible states in a pool. */
Floorplanner::GaussianStats Floorplanner::computeGaussianStats(const vector<SolutionState> &pool) const
{
    vector<double> areas;
    vector<double> wires;
    areas.reserve(pool.size());
    wires.reserve(pool.size());

    for (const SolutionState &state : pool)
    {
        if (!state.valid || !state.feasible)
        {
            continue;
        }
        areas.push_back(static_cast<double>(state.area));
        wires.push_back(state.wireLength);
    }

    GaussianStats stats;
    if (areas.size() < 2)
    {
        return stats;
    }

    auto meanOf = [](const vector<double> &values) {
        double sum = 0.0;
        for (double value : values)
        {
            sum += value;
        }
        return sum / static_cast<double>(values.size());
    };

    auto stdOf = [](const vector<double> &values, double mean) {
        double sum = 0.0;
        for (double value : values)
        {
            const double delta = value - mean;
            sum += delta * delta;
        }
        return std::sqrt(sum / static_cast<double>(values.size()));
    };

    stats.areaMean = meanOf(areas);
    stats.wireMean = meanOf(wires);
    stats.areaStd = stdOf(areas, stats.areaMean);
    stats.wireStd = stdOf(wires, stats.wireMean);
    if (stats.areaStd < 1e-9)
    {
        stats.areaStd = 1.0;
    }
    if (stats.wireStd < 1e-9)
    {
        stats.wireStd = 1.0;
    }
    stats.valid = true;
    return stats;
}

/** Score a feasible state against the Gaussian model when available. */
double Floorplanner::calcGaussianFeasibleScore(const SolutionState &state, const GaussianStats &stats) const
{
    if (!stats.valid)
    {
        return chooseBiasedRankstate.reportCost;
    }

    const double zArea = (static_cast<double>(state.area) - stats.areaMean) / stats.areaStd;
    const double zWire = (state.wireLength - stats.wireMean) / stats.wireStd;
    return _metricAlpha * zArea + (1.0 - _metricAlpha) * zWire;
}

/**
 * Decide whether the current outer round has effectively converged.
 * The check compares the previous and current pools using either Gaussian
 * feasible scores or the raw search/report costs when too few feasible states
 * are available.
 */
bool Floorplanner::shouldStopRound(const vector<SolutionState> &prevPool,
                                   const vector<SolutionState> &currPool) const
{
    vector<SolutionState> merged;
    merged.reserve(prevPool.size() + currPool.size());
    merged.insert(merged.end(), prevPool.begin(), prevPool.end());
    merged.insert(merged.end(), currPool.begin(), currPool.end());
    const GaussianStats stats = computeGaussianStats(merged);
    if (stats.valid)
    {
        auto collectGaussianCosts = [&](const vector<SolutionState> &pool) {
            vector<double> feasibleCosts;
            for (const SolutionState &state : pool)
            {
                if (!state.valid || !state.feasible)
                {
                    continue;
                }
                feasibleCosts.push_back(calcGaussianFeasibleScore(state, stats));
            }
            return feasibleCosts;
        };

        vector<double> prevCosts = collectGaussianCosts(prevPool);
        vector<double> currCosts = collectGaussianCosts(currPool);
        if (!prevCosts.empty() && !currCosts.empty())
        {
            const double prevMedian = medianOf(prevCosts);
            const double currMedian = medianOf(currCosts);
            const double prevBest = *min_element(prevCosts.begin(), prevCosts.end());
            const double currBest = *min_element(currCosts.begin(), currCosts.end());
            const double improveMedian = (prevMedian - currMedian) / max(std::fabs(prevMedian), 1e-9);
            const double improveBest = (prevBest - currBest) / max(std::fabs(prevBest), 1e-9);
            return improveMedian < 0.01 && improveBest < 0.005;
        }
    }

    auto collectCosts = [](const vector<SolutionState> &pool) {
        vector<double> feasibleCosts;
        vector<double> allCosts;
        for (const SolutionState &state : pool)
        {
            if (!state.valid)
            {
                continue;
            }
            allCosts.push_back(state.searchCost);
            if (state.feasible)
            {
                feasibleCosts.push_back(state.reportCost);
            }
        }
        return feasibleCosts.empty() ? allCosts : feasibleCosts;
    };

    vector<double> prevCosts = collectCosts(prevPool);
    vector<double> currCosts = collectCosts(currPool);
    if (prevCosts.empty() || currCosts.empty())
    {
        return false;
    }

    const double prevMedian = medianOf(prevCosts);
    const double currMedian = medianOf(currCosts);
    const double prevBest = *min_element(prevCosts.begin(), prevCosts.end());
    const double currBest = *min_element(currCosts.begin(), currCosts.end());

    const double improveMedian = (prevMedian - currMedian) / max(prevMedian, 1e-9);
    const double improveBest = (prevBest - currBest) / max(prevBest, 1e-9);

    return improveMedian < 0.01 && improveBest < 0.005;
}
