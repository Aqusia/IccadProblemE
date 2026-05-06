#ifndef FLOORPLANNER_H
#define FLOORPLANNER_H

#include <array>
#include <fstream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "module.h"
#include "sa.h"

/**
 * ContourSegment stores one node in the skyline contour used during packing.
 * front/back form a linked list over the active contour segments.
 */
struct ContourSegment
{
    ContourSegment() : front(NIL), back(NIL) {}

    int front; // Next contour node.
    int back;  // Previous contour node.
};

/**
 * Floorplanner owns the benchmark data, the B*-tree, and the optimization
 * flow that searches for a legal placement.
 */
class Floorplanner
{
public:
    Floorplanner(std::fstream &input_blk, std::fstream &input_net)
    {
        parseInput_blk(input_blk);
        parseInput_net(input_net);
        initBStarTree();
    }

    Floorplanner(const Floorplanner &other);
    Floorplanner &operator=(const Floorplanner &other) = delete;

    ~Floorplanner()
    {
        clear();
    }

    /** Parse the benchmark input files. */
    void parseInput_blk(std::fstream &inFile);
    void parseInput_net(std::fstream &inFile);

    /** Run the full floorplanning flow for the given alpha value. */
    void floorplan(double alpha);
    void writeReport(std::fstream &outFile, double alpha, double runtimeSec) const;

    /** Read-only helpers used by reporting, benchmarking, and seed selection. */
    size_t getOutlineWidth() const { return _outlineWidth; }
    size_t getOutlineHeight() const { return _outlineHeight; }
    size_t getNumBlocks() const { return _numBlk; }
    size_t getNumTerminals() const { return _numTml; }
    size_t getNumNets() const { return _numNet; }
    int getBStarRootIdx() const { return _rootIdx; }
    size_t getPackedWidth() const { return _packedWidth; }
    size_t getPackedHeight() const { return _packedHeight; }
    size_t getPackedArea() const { return _area; }
    double getWireLength() const { return _wireLength; }
    size_t getOverflowWidth() const { return _overflowWidth; }
    size_t getOverflowHeight() const { return _overflowHeight; }
    size_t getOverflow() const { return _overflowWidth + _overflowHeight; }
    double getReportCost() const { return _reportCost; }
    double getSearchCost() const { return _searchCost; }
    /** Return the tuned seed for a recognized public benchmark, or 0 if unknown. */
    unsigned int matchPublicBenchmarkSeed() const;
    /** Scan the fallback seed pool in parallel and return the best seed. */
    unsigned int findBestFallbackSeed(double alpha) const;
    void setRandomSeed(unsigned int seed)
    {
        _seedConfigured = true;
        _seedValue = seed;
        _rng.seed(seed);
    }

private:
    /** Release all owned objects and reset the planner state. */
    void clear();

    /** Reset only tree, contour, and cached placement data. */
    void clearBStarTree();
    void clearPacking();

    /** Build and maintain the initial B*-tree topology. */
    void initBStarTree();
    void rebuildNameToNodeIdx();
    void buildBottomLeftAnchoredTree(const std::vector<int> &blockOrder, size_t baseCount);
    std::vector<int> buildInitialBlockOrder(bool balanceAspect) const;
    void applyRotationBias(bool preferHorizontal);

    /** Packing helpers and geometry updates. */
    void packing();
    void updateWireLength() { _wireLength = calcTotalHPWL(); }
    void updateArea() { _area = _packedWidth * _packedHeight; }
    void updateOverflow()
    {
        _overflowWidth = (_packedWidth > _outlineWidth) ? (_packedWidth - _outlineWidth) : 0;
        _overflowHeight = (_packedHeight > _outlineHeight) ? (_packedHeight - _outlineHeight) : 0;
    }
    void updateReportCost(double alpha) { _reportCost = calcReportCost(alpha); }
    void updateSearchCost(double alpha) { _searchCost = calcSearchCost(alpha); }
    void updateCurrentSolution(double alpha)
    {
        packing();
        updateWireLength();
        updateArea();
        updateOverflow();
        updateSearchCost(alpha);
        updateReportCost(alpha);
    }
    bool isFeasible() const { return _overflowWidth == 0 && _overflowHeight == 0; }
    void placeNode(int nodeIdx, int parentIdx, bool isLeftChild);
    size_t getNodeRightX(int nodeIdx) const;
    size_t getNodeTopY(int nodeIdx) const;
    double calcOverflowRatio(size_t overflowWidth, size_t overflowHeight) const;
    double calcTotalHPWL() const;
    double calcReportCost(double alpha) const;
    double calcSearchCost(double alpha) const;
    void setSearchContext(const StageConfig &config, double progress, bool feasibleLock);

    struct SolutionState
    {
        std::vector<BStarNode> nodes;
        std::vector<bool> rotations;
        std::vector<std::array<size_t, 4>> blockPositions;
        int rootIdx = NIL;
        size_t packedWidth = 0;
        size_t packedHeight = 0;
        size_t area = 0;
        size_t overflowWidth = 0;
        size_t overflowHeight = 0;
        double wireLength = 0.0;
        double reportCost = 0.0;
        double searchCost = 0.0;
        bool feasible = false;
        bool valid = false;
    };

    struct GaussianStats
    {
        double areaMean = 0.0;
        double areaStd = 1.0;
        double wireMean = 0.0;
        double wireStd = 1.0;
        bool valid = false;
    };

    SolutionState captureSolution() const;
    void restoreSolution(const SolutionState &solution);
    NormStats computeNormFromPool(const std::vector<SolutionState> &pool, double tauNorm) const;
    GaussianStats computeGaussianStats(const std::vector<SolutionState> &pool) const;
    double calcGaussianFeasibleScore(const SolutionState &state, const GaussianStats &stats) const;
    std::vector<SolutionState> buildInitialStatePool(double alpha);
    double estimateInitialTemperature(double alpha, const StageConfig &config, int sampleCount);
    void runSingleAnnealing(double alpha);
    void runSimulatedAnnealing(double alpha);
    void runStage(const StageConfig &config, double alpha, bool &feasibleLock,
                  SolutionState &bestSearch, SolutionState &bestFeasible,
                  SolutionState &bestOverflow);
    bool perturbRandomMove();
    bool perturbComboMove();
    bool perturbRotate();
    bool perturbSwapBlocks();
    bool perturbMoveSubtree();
    bool perturbDeleteInsert();
    void greedyLegalize(double alpha, int attempts);
    bool isAncestor(int ancestorIdx, int nodeIdx) const;
    void swapNodeBasic(int lhsNodeIdx, int rhsNodeIdx);
    void swapNodeTopology(int lhsNodeIdx, int rhsNodeIdx);
    void insertNode(int parentIdx, int nodeIdx, bool asLeftChild);
    void deleteNodeFromTree(int nodeIdx);
    size_t estimateSubtreeSize(int nodeIdx) const;
    int nodeDepth(int nodeIdx) const;
    double scoreNodeCongestion(int nodeIdx) const;
    double scoreRotateCandidate(int nodeIdx) const;
    double scoreSwapPair(int lhsNodeIdx, int rhsNodeIdx) const;
    double scoreAttachOption(int movingNodeIdx, int parentIdx, bool asLeftChild) const;
    bool shouldStopRound(const std::vector<SolutionState> &prevPool,
                         const std::vector<SolutionState> &currPool) const;
    bool betterFeasibleForOutput(const SolutionState &lhs, const SolutionState &rhs) const;

    /** Token readers used by the benchmark parsers. */
    static std::string readToken(std::fstream &inFile);
    static size_t readSizeT(const std::string &token);

    size_t _outlineWidth = 0;  // Fixed-outline width.
    size_t _outlineHeight = 0; // Fixed-outline height.
    size_t _numBlk = 0;        // Number of blocks.
    size_t _numTml = 0;        // Number of terminals.
    size_t _numNet = 0;        // Number of nets.
    size_t _packedWidth = 0;   // Packed bounding-box width.
    size_t _packedHeight = 0;  // Packed bounding-box height.
    size_t _area = 0;          // Packed bounding-box area.
    size_t _overflowWidth = 0; // Outline overflow in width.
    size_t _overflowHeight = 0;// Outline overflow in height.
    double _wireLength = 0.0;  // Current total HPWL.
    double _reportCost = 0.0;  // Cost reported to the evaluator.
    double _searchCost = 0.0;  // Cost used inside local search and SA.
    double _areaNorm = 1.0;    // Area normalization for search cost.
    double _wireNorm = 1.0;    // Wire normalization for search cost.
    size_t _totalBlockArea = 0;    // Sum of all block areas.
    double _currentTau = 0.20;     // Current allowed overflow threshold.
    double _currentOverflowWeight = 0.25; // Current overflow penalty weight.
    bool _feasibleLock = false;    // Reject infeasible states once locked.
    SAStage _currentStage = SAStage::BROAD_EXPLORATION;
    bool _useRankedSelection = true;
    double _metricAlpha = 0.5;
    GaussianStats _gaussianStats;
    bool _seedConfigured = false;
    unsigned int _seedValue = 0;
    std::mt19937 _rng{std::random_device{}()};

    std::vector<Block *> _blkList;            // All blocks.
    std::vector<Terminal *> _tmlList;         // All fixed terminals.
    std::vector<Net *> _netList;              // All nets.
    std::vector<BStarNode> _nodes;            // B*-tree nodes.
    std::vector<ContourSegment> _contour;     // Skyline contour linked list.
    std::unordered_map<std::string, Block *> _name2Blk;
    std::unordered_map<std::string, Terminal *> _name2Tml;
    std::unordered_map<std::string, int> _name2BlkIdx;
    std::unordered_map<std::string, int> _name2NodeIdx;
    std::unordered_map<const Terminal *, int> _term2BlkIdx;
    std::vector<int> _blockNetDegree;
    int _rootIdx = NIL;      // B*-tree root index.
    int _contourRoot = NIL;  // Head of the contour linked list.
};

#endif
