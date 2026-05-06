#include "floorplanner.h"

#include <algorithm>
#include <cmath>

using namespace std;

/** Return a uniform random value in [0, 1). */
static double rand01(mt19937 &rng)
{
    uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

/** Choose a ranked candidate with a bias toward the top few options. */
static int chooseBiasedRank(int candidateCount, mt19937 &rng)
{
    if (candidateCount <= 1)
    {
        return 0;
    }

    static const double weights[] = {0.50, 0.27, 0.15, 0.08};
    const int usable = min(candidateCount, 4);
    double total = 0.0;
    for (int i = 0; i < usable; ++i)
    {
        total += weights[i];
    }

    uniform_real_distribution<double> dist(0.0, total);
    double pick = dist(rng);
    for (int i = 0; i < usable; ++i)
    {
        pick -= weights[i];
        if (pick <= 0.0)
        {
            return i;
        }
    }
    return usable - 1;
}

/** Return the depth of one node in the current B*-tree. */
int Floorplanner::nodeDepth(int nodeIdx) const
{
    int depth = 0;
    for (int cur = nodeIdx; cur != NIL && _nodes[cur].parent != NIL; cur = _nodes[cur].parent)
    {
        ++depth;
    }
    return depth;
}

/** Estimate the size of a subtree rooted at nodeIdx. */
size_t Floorplanner::estimateSubtreeSize(int nodeIdx) const
{
    if (nodeIdx == NIL)
    {
        return 0;
    }

    size_t size = 0;
    vector<int> stack(1, nodeIdx);
    while (!stack.empty())
    {
        const int cur = stack.back();
        stack.pop_back();
        ++size;
        if (_nodes[cur].left != NIL)
        {
            stack.push_back(_nodes[cur].left);
        }
        if (_nodes[cur].right != NIL)
        {
            stack.push_back(_nodes[cur].right);
        }
    }
    return size;
}

/**
 * Estimate how much one node contributes to the current congestion pressure.
 * The score mixes outline overflow pressure, aspect-ratio mismatch, and the
 * node's normalized geometric position in the packed layout.
 */
double Floorplanner::scoreNodeCongestion(int nodeIdx) const
{
    const Block *blk = _blkList[_nodes[nodeIdx].blockIdx];
    const double widthPressure =
        max(0.0, static_cast<double>(_packedWidth) - static_cast<double>(_outlineWidth)) /
        max(static_cast<double>(_outlineWidth), 1.0);
    const double heightPressure =
        max(0.0, static_cast<double>(_packedHeight) - static_cast<double>(_outlineHeight)) /
        max(static_cast<double>(_outlineHeight), 1.0);

    double widthBias = widthPressure;
    double heightBias = heightPressure;
    if (widthBias <= 1e-12 && heightBias <= 1e-12)
    {
        const double targetAspect =
            max(static_cast<double>(_outlineWidth), 1.0) / max(static_cast<double>(_outlineHeight), 1.0);
        const double currentAspect =
            max(static_cast<double>(_packedWidth), 1.0) / max(static_cast<double>(_packedHeight), 1.0);
        if (currentAspect >= targetAspect)
        {
            widthBias = currentAspect - targetAspect;
        }
        else
        {
            heightBias = targetAspect - currentAspect;
        }
    }

    const double xNorm = static_cast<double>(blk->getX2()) / max(static_cast<double>(_packedWidth), 1.0);
    const double yNorm = static_cast<double>(blk->getY2()) / max(static_cast<double>(_packedHeight), 1.0);
    return widthBias * xNorm + heightBias * yNorm + 0.15 * (xNorm + yNorm);
}

/**
 * Estimate whether rotating one block is likely to help.
 * The heuristic rewards rotations that relieve the dominant width/height
 * pressure while also favoring congested and highly connected blocks.
 */
double Floorplanner::scoreRotateCandidate(int nodeIdx) const
{
    const Block *blk = _blkList[_nodes[nodeIdx].blockIdx];
    const double width = static_cast<double>(blk->getCurrentWidth());
    const double height = static_cast<double>(blk->getCurrentHeight());
    const double rotatedWidth = height;
    const double rotatedHeight = width;
    const double widthPressure =
        max(0.0, static_cast<double>(_packedWidth) - static_cast<double>(_outlineWidth)) /
        max(static_cast<double>(_outlineWidth), 1.0);
    const double heightPressure =
        max(0.0, static_cast<double>(_packedHeight) - static_cast<double>(_outlineHeight)) /
        max(static_cast<double>(_outlineHeight), 1.0);

    double dominantWidth = widthPressure;
    double dominantHeight = heightPressure;
    if (dominantWidth <= 1e-12 && dominantHeight <= 1e-12)
    {
        const double targetAspect =
            max(static_cast<double>(_outlineWidth), 1.0) / max(static_cast<double>(_outlineHeight), 1.0);
        const double currentAspect =
            max(static_cast<double>(_packedWidth), 1.0) / max(static_cast<double>(_packedHeight), 1.0);
        if (currentAspect >= targetAspect)
        {
            dominantWidth = currentAspect - targetAspect;
        }
        else
        {
            dominantHeight = targetAspect - currentAspect;
        }
    }

    const double pressureGain =
        dominantWidth * (width - rotatedWidth) + dominantHeight * (height - rotatedHeight);

    double score = 1.25 * pressureGain;
    score += 0.75 * scoreNodeCongestion(nodeIdx);
    score += 0.03 * _blockNetDegree[_nodes[nodeIdx].blockIdx];
    return score;
}

/**
 * Estimate whether swapping two nodes is worth trying.
 * The score favors pairs whose congestion roles and geometric "mass" suggest
 * that exchanging their positions could better balance the tree.
 */
double Floorplanner::scoreSwapPair(int lhsNodeIdx, int rhsNodeIdx) const
{
    const Block *lhsBlk = _blkList[_nodes[lhsNodeIdx].blockIdx];
    const Block *rhsBlk = _blkList[_nodes[rhsNodeIdx].blockIdx];

    const double lhsCongestion = scoreNodeCongestion(lhsNodeIdx);
    const double rhsCongestion = scoreNodeCongestion(rhsNodeIdx);
    const double lhsMass = sqrt(static_cast<double>(lhsBlk->getArea()));
    const double rhsMass = sqrt(static_cast<double>(rhsBlk->getArea()));
    const double lhsDegree = static_cast<double>(_blockNetDegree[_nodes[lhsNodeIdx].blockIdx]);
    const double rhsDegree = static_cast<double>(_blockNetDegree[_nodes[rhsNodeIdx].blockIdx]);
    const double sizeSimilarity = min(lhsMass, rhsMass) / max(max(lhsMass, rhsMass), 1e-9);

    const double congestionGap = fabs(lhsCongestion - rhsCongestion);
    const double payloadGap = fabs((lhsMass + 0.15 * lhsDegree) - (rhsMass + 0.15 * rhsDegree));
    const bool aligned =
        ((lhsCongestion - rhsCongestion) * ((lhsMass + 0.15 * lhsDegree) - (rhsMass + 0.15 * rhsDegree))) > 0.0;
    const bool parentChild =
        (_nodes[lhsNodeIdx].parent == rhsNodeIdx) || (_nodes[rhsNodeIdx].parent == lhsNodeIdx);
    const bool sibling =
        (_nodes[lhsNodeIdx].parent != NIL) && (_nodes[lhsNodeIdx].parent == _nodes[rhsNodeIdx].parent);
    const double depthGap = fabs(static_cast<double>(nodeDepth(lhsNodeIdx) - nodeDepth(rhsNodeIdx)));

    double score = congestionGap * (1.0 + payloadGap) * (0.55 + 0.45 * sizeSimilarity);
    if (!aligned)
    {
        score *= 0.25;
    }
    if (_currentStage != SAStage::BROAD_EXPLORATION && sizeSimilarity < 0.35)
    {
        score *= 0.45;
    }
    if (_currentStage == SAStage::REFINEMENT)
    {
        score += 0.35 * sizeSimilarity;
        if (parentChild)
        {
            score += 0.75;
        }
        else if (sibling)
        {
            score += 0.35;
        }
        score += max(0.0, 0.30 - 0.05 * depthGap);
    }
    return score;
}

/**
 * Score one possible reattachment target for a moving subtree.
 * The score reflects outline pressure, estimated position, and whether the
 * candidate parent keeps important blocks closer to the center.
 */
double Floorplanner::scoreAttachOption(int movingNodeIdx, int parentIdx, bool asLeftChild) const
{
    const Block *parentBlk = _blkList[_nodes[parentIdx].blockIdx];
    const double estX = asLeftChild ? static_cast<double>(parentBlk->getX2())
                                    : static_cast<double>(parentBlk->getX1());
    const double estY = asLeftChild ? static_cast<double>(parentBlk->getY1())
                                    : static_cast<double>(parentBlk->getY2());

    const double widthPressure =
        max(0.0, static_cast<double>(_packedWidth) - static_cast<double>(_outlineWidth)) /
        max(static_cast<double>(_outlineWidth), 1.0);
    const double heightPressure =
        max(0.0, static_cast<double>(_packedHeight) - static_cast<double>(_outlineHeight)) /
        max(static_cast<double>(_outlineHeight), 1.0);

    const double xNorm = estX / max(static_cast<double>(_packedWidth), 1.0);
    const double yNorm = estY / max(static_cast<double>(_packedHeight), 1.0);
    const double centerPenalty = fabs(xNorm - 0.5) + fabs(yNorm - 0.5);
    const double degreeWeight =
        min(1.0, static_cast<double>(_blockNetDegree[_nodes[movingNodeIdx].blockIdx]) / 8.0);

    double score = 0.0;
    if (widthPressure >= heightPressure)
    {
        score += asLeftChild ? -0.35 : 0.35;
        score += 1.0 - xNorm;
    }
    else
    {
        score += asLeftChild ? 0.35 : -0.35;
        score += 1.0 - yNorm;
    }

    score += 0.20 * (1.0 - min(scoreNodeCongestion(parentIdx), 1.0));
    score -= 0.18 * degreeWeight * centerPenalty;
    score -= 0.02 * static_cast<double>(_blockNetDegree[_nodes[movingNodeIdx].blockIdx]) * (xNorm + yNorm);
    return score;
}

/**
 * Sample and apply one perturbation according to the current SA stage.
 * Exploration favors larger topology edits, while refinement prefers more
 * targeted rotations and swaps.
 */
bool Floorplanner::perturbRandomMove()
{
    for (int attempt = 0; attempt < 6; ++attempt)
    {
        const double pick = rand01(_rng);
        bool changed = false;

        if (_currentStage == SAStage::REFINEMENT)
        {
            if (pick < 0.20)
            {
                changed = perturbComboMove();
            }
            else if (pick < 0.35)
            {
                changed = perturbRotate();
            }
            else if (pick < 0.60)
            {
                changed = perturbSwapBlocks();
            }
            else
            {
                changed = perturbMoveSubtree();
            }
        }
        else if (_currentStage == SAStage::DIVERSIFICATION)
        {
            if (pick < 0.20)
            {
                changed = perturbRotate();
            }
            else if (pick < 0.45)
            {
                changed = perturbSwapBlocks();
            }
            else
            {
                changed = perturbMoveSubtree();
            }
        }
        else
        {
            if (pick < 0.20)
            {
                changed = perturbRotate();
            }
            else if (pick < 0.50)
            {
                changed = perturbSwapBlocks();
            }
            else
            {
                changed = perturbMoveSubtree();
            }
        }

        if (changed)
        {
            return true;
        }
    }
    return false;
}

/**
 * Compose a rotate with one additional structural move as a macro move.
 * This gives refinement stages a way to escape shallow local minima without
 * committing to a full random restart.
 */
bool Floorplanner::perturbComboMove()
{
    if (_nodes.size() < 2)
    {
        return perturbRotate();
    }

    const SolutionState base = captureSolution();
    
    if (!perturbRotate())
    {
        return false;
    }

    bool changed = false;
    const double comboPick = rand01(_rng);
    if (comboPick < 0.55)
    {
        changed = perturbSwapBlocks();
    }
    else
    {
        if (rand01(_rng) < 0.45)
        {
            if (!perturbRotate())
            {
                restoreSolution(base);
                return false;
            }
        }
        changed = perturbMoveSubtree();
    }

    if (!changed)
    {
        restoreSolution(base);
        return false;
    }
    return true;
}

/**
 * Rotate one block chosen from a scored candidate set.
 * A small random sample is scored first, then one candidate is selected from
 * the ranked list.
 */
bool Floorplanner::perturbRotate()
{
    if (_nodes.empty())
    {
        return false;
    }

    const int sampleCount = min<int>(16, static_cast<int>(_nodes.size()));
    uniform_int_distribution<int> nodeDist(0, static_cast<int>(_nodes.size()) - 1);
    vector<pair<double, int>> ranked;
    ranked.reserve(sampleCount);
    for (int i = 0; i < sampleCount; ++i)
    {
        const int nodeIdx = nodeDist(_rng);
        ranked.push_back({scoreRotateCandidate(nodeIdx), nodeIdx});
    }

    sort(ranked.begin(), ranked.end(),
         [](const pair<double, int> &lhs, const pair<double, int> &rhs) {
             if (fabs(lhs.first - rhs.first) > 1e-12)
             {
                 return lhs.first > rhs.first;
             }
             return lhs.second < rhs.second;
         });
    int chosenRank = 0;
    if (_useRankedSelection)
    {
        chosenRank = chooseBiasedRank(static_cast<int>(ranked.size()), _rng);
    }
    else
    {
        uniform_int_distribution<int> rankDist(0, static_cast<int>(ranked.size()) - 1);
        chosenRank = rankDist(_rng);
    }
    const int bestNode = ranked[chosenRank].second;
    _blkList[_nodes[bestNode].blockIdx]->setRotate();
    return true;
}

/**
 * Swap the topology positions of two sampled nodes.
 * The move ranks sampled pairs first so the solver spends more time on swaps
 * that are likely to improve pressure or local structure.
 */
bool Floorplanner::perturbSwapBlocks()
{
    if (_nodes.size() < 2)
    {
        return false;
    }

    const int sampleCount = min<int>(16, static_cast<int>(_nodes.size()) * 2);
    uniform_int_distribution<int> nodeDist(0, static_cast<int>(_nodes.size()) - 1);

    struct SwapCandidate
    {
        double score;
        int lhs;
        int rhs;
    };
    vector<SwapCandidate> ranked;
    ranked.reserve(sampleCount);
    const bool useRanked =
        _useRankedSelection &&
        (_currentStage == SAStage::REFINEMENT || _currentStage == SAStage::DIVERSIFICATION);

    for (int i = 0; i < sampleCount; ++i)
    {
        int lhs = nodeDist(_rng);
        int rhs = lhs;

        vector<int> nearby;
        const double localSwapChance =
            (_currentStage == SAStage::REFINEMENT) ? 0.70
            : (_currentStage == SAStage::DIVERSIFICATION) ? 0.40
                                                          : 0.0;
        if (localSwapChance > 0.0 && rand01(_rng) < localSwapChance)
        {
            const int parent = _nodes[lhs].parent;
            if (parent != NIL)
            {
                nearby.push_back(parent);
                const int sibling =
                    (_nodes[parent].left == lhs) ? _nodes[parent].right : _nodes[parent].left;
                if (sibling != NIL)
                {
                    nearby.push_back(sibling);
                }
                if (_nodes[parent].parent != NIL)
                {
                    nearby.push_back(_nodes[parent].parent);
                }
            }
            if (_nodes[lhs].left != NIL)
            {
                nearby.push_back(_nodes[lhs].left);
            }
            if (_nodes[lhs].right != NIL)
            {
                nearby.push_back(_nodes[lhs].right);
            }
        }

        if (!nearby.empty())
        {
            uniform_int_distribution<int> nearDist(0, static_cast<int>(nearby.size()) - 1);
            rhs = nearby[nearDist(_rng)];
        }
        while (rhs == lhs)
        {
            rhs = nodeDist(_rng);
        }

        const double score = scoreSwapPair(lhs, rhs);
        ranked.push_back({score, lhs, rhs});
    }

    sort(ranked.begin(), ranked.end(),
         [](const SwapCandidate &lhs, const SwapCandidate &rhs) {
             if (fabs(lhs.score - rhs.score) > 1e-12)
             {
                 return lhs.score > rhs.score;
             }
             if (lhs.lhs != rhs.lhs)
             {
                 return lhs.lhs < rhs.lhs;
             }
             return lhs.rhs < rhs.rhs;
         });
    int chosenRank = 0;
    if (useRanked)
    {
        chosenRank = chooseBiasedRank(static_cast<int>(ranked.size()), _rng);
    }
    else
    {
        uniform_int_distribution<int> rankDist(0, static_cast<int>(ranked.size()) - 1);
        chosenRank = rankDist(_rng);
    }
    const int bestLhs = ranked[chosenRank].lhs;
    const int bestRhs = ranked[chosenRank].rhs;
    swapNodeTopology(bestLhs, bestRhs);
    return true;
}

/** Return true when ancestorIdx lies on the path to nodeIdx. */
bool Floorplanner::isAncestor(int ancestorIdx, int nodeIdx) const
{
    int cur = nodeIdx;
    while (cur != NIL)
    {
        if (cur == ancestorIdx)
        {
            return true;
        }
        cur = _nodes[cur].parent;
    }
    return false;
}

/** Swap two nodes assuming no ancestor relationship between them. */
void Floorplanner::swapNodeBasic(int lhsNodeIdx, int rhsNodeIdx)
{
    BStarNode &lhs = _nodes[lhsNodeIdx];
    BStarNode &rhs = _nodes[rhsNodeIdx];

    if (lhs.left != NIL)
    {
        _nodes[lhs.left].parent = rhsNodeIdx;
    }
    if (lhs.right != NIL)
    {
        _nodes[lhs.right].parent = rhsNodeIdx;
    }
    if (rhs.left != NIL)
    {
        _nodes[rhs.left].parent = lhsNodeIdx;
    }
    if (rhs.right != NIL)
    {
        _nodes[rhs.right].parent = lhsNodeIdx;
    }

    if (lhs.parent != lhsNodeIdx)
    {
        if (lhs.parent != NIL)
        {
            if (_nodes[lhs.parent].left == lhsNodeIdx)
            {
                _nodes[lhs.parent].left = rhsNodeIdx;
            }
            else
            {
                _nodes[lhs.parent].right = rhsNodeIdx;
            }
        }
        else
        {
            _rootIdx = rhsNodeIdx;
        }
    }

    if (rhs.parent != rhsNodeIdx)
    {
        if (rhs.parent != NIL)
        {
            if (_nodes[rhs.parent].left == rhsNodeIdx)
            {
                _nodes[rhs.parent].left = lhsNodeIdx;
            }
            else
            {
                _nodes[rhs.parent].right = lhsNodeIdx;
            }
        }
        else
        {
            _rootIdx = lhsNodeIdx;
        }
    }

    swap(lhs.left, rhs.left);
    swap(lhs.right, rhs.right);
    swap(lhs.parent, rhs.parent);
}

/**
 * Swap two nodes while handling parent-child edge cases safely.
 * This wrapper redirects direct parent-child swaps into a safe sequence before
 * falling back to the generic swapNodeBasic path.
 */
void Floorplanner::swapNodeTopology(int lhsNodeIdx, int rhsNodeIdx)
{
    if (lhsNodeIdx == rhsNodeIdx || lhsNodeIdx == NIL || rhsNodeIdx == NIL)
    {
        return;
    }

    if (_nodes[lhsNodeIdx].parent != rhsNodeIdx && _nodes[rhsNodeIdx].parent != lhsNodeIdx)
    {
        swapNodeBasic(lhsNodeIdx, rhsNodeIdx);
        return;
    }

    const bool lhsIsParent = (lhsNodeIdx == _nodes[rhsNodeIdx].parent);
    bool leftChild = false;

    if (lhsIsParent)
    {
        if (_nodes[lhsNodeIdx].left == rhsNodeIdx)
        {
            _nodes[lhsNodeIdx].left = NIL;
            leftChild = true;
        }
        else
        {
            _nodes[lhsNodeIdx].right = NIL;
            leftChild = false;
        }
        _nodes[rhsNodeIdx].parent = rhsNodeIdx;
    }
    else
    {
        if (_nodes[rhsNodeIdx].left == lhsNodeIdx)
        {
            _nodes[rhsNodeIdx].left = NIL;
            leftChild = true;
        }
        else
        {
            _nodes[rhsNodeIdx].right = NIL;
            leftChild = false;
        }
        _nodes[lhsNodeIdx].parent = lhsNodeIdx;
    }

    swapNodeBasic(lhsNodeIdx, rhsNodeIdx);
    if (lhsIsParent)
    {
        _nodes[lhsNodeIdx].parent = rhsNodeIdx;
        if (leftChild)
        {
            _nodes[rhsNodeIdx].left = lhsNodeIdx;
        }
        else
        {
            _nodes[rhsNodeIdx].right = lhsNodeIdx;
        }
    }
    else
    {
        _nodes[rhsNodeIdx].parent = lhsNodeIdx;
        if (leftChild)
        {
            _nodes[lhsNodeIdx].left = rhsNodeIdx;
        }
        else
        {
            _nodes[lhsNodeIdx].right = rhsNodeIdx;
        }
    }
}

/**
 * Insert one detached node under the chosen parent edge.
 * The node becomes the direct child of parentIdx and adopts the displaced
 * child subtree, if any, on the corresponding side.
 */
void Floorplanner::insertNode(int parentIdx, int nodeIdx, bool asLeftChild)
{
    BStarNode &parent = _nodes[parentIdx];
    BStarNode &node = _nodes[nodeIdx];
    node.parent = parentIdx;

    if (asLeftChild)
    {
        node.left = parent.left;
        node.right = NIL;
        if (parent.left != NIL)
        {
            _nodes[parent.left].parent = nodeIdx;
        }
        parent.left = nodeIdx;
    }
    else
    {
        node.left = NIL;
        node.right = parent.right;
        if (parent.right != NIL)
        {
            _nodes[parent.right].parent = nodeIdx;
        }
        parent.right = nodeIdx;
    }
}

/**
 * Remove one node from the tree while preserving the remaining topology.
 * The function reconnects surviving children so the tree stays connected and
 * still contains every other block exactly once.
 */
void Floorplanner::deleteNodeFromTree(int nodeIdx)
{
    BStarNode &node = _nodes[nodeIdx];
    int child = NIL;
    int subchild = NIL;
    int subparent = NIL;

    if (!node.isLeaf())
    {
        bool pullLeft = rand01(_rng) < 0.5;
        if (node.left == NIL)
        {
            pullLeft = false;
        }
        if (node.right == NIL)
        {
            pullLeft = true;
        }

        if (pullLeft)
        {
            child = node.left;
            if (node.right != NIL)
            {
                subchild = _nodes[child].right;
                subparent = node.right;
                _nodes[node.right].parent = child;
                _nodes[child].right = node.right;
            }
        }
        else
        {
            child = node.right;
            if (node.left != NIL)
            {
                subchild = _nodes[child].left;
                subparent = node.left;
                _nodes[node.left].parent = child;
                _nodes[child].left = node.left;
            }
        }

        _nodes[child].parent = node.parent;
    }

    if (node.parent == NIL)
    {
        _rootIdx = child;
    }
    else if (_nodes[node.parent].left == nodeIdx)
    {
        _nodes[node.parent].left = child;
    }
    else
    {
        _nodes[node.parent].right = child;
    }

    if (subchild != NIL)
    {
        int current = subparent;
        while (true)
        {
            BStarNode &parent = _nodes[current];
            if (parent.left == NIL && parent.right == NIL)
            {
                _nodes[subchild].parent = current;
                if (rand01(_rng) < 0.5)
                {
                    parent.left = subchild;
                }
                else
                {
                    parent.right = subchild;
                }
                break;
            }
            if (parent.left == NIL)
            {
                _nodes[subchild].parent = current;
                parent.left = subchild;
                break;
            }
            if (parent.right == NIL)
            {
                _nodes[subchild].parent = current;
                parent.right = subchild;
                break;
            }
            current = (rand01(_rng) < 0.5) ? parent.left : parent.right;
        }
    }

    node.parent = NIL;
    node.left = NIL;
    node.right = NIL;
}

/**
 * Delete one subtree root and reinsert it somewhere else.
 * This move is the more exploratory subtree edit and is used heavily in early
 * stages or diversification.
 */
bool Floorplanner::perturbDeleteInsert()
{
    if (_nodes.size() < 2)
    {
        return false;
    }

    const int sampleCount = min<int>(24, static_cast<int>(_nodes.size()));
    uniform_int_distribution<int> nodeDist(0, static_cast<int>(_nodes.size()) - 1);

    vector<pair<double, int>> movingCandidates;
    movingCandidates.reserve(sampleCount);
    for (int i = 0; i < sampleCount; ++i)
    {
        const int candidate = nodeDist(_rng);
        const double rootPenalty = (candidate == _rootIdx) ? 0.35 : 0.0;
        const double leafBonus = _nodes[candidate].isLeaf() ? 0.80 : 0.0;
        const double depthBonus = 0.18 * nodeDepth(candidate);
        const double subtreePenalty = 0.10 * static_cast<double>(estimateSubtreeSize(candidate));
        const double score = scoreNodeCongestion(candidate) + leafBonus + depthBonus - subtreePenalty - rootPenalty;
        movingCandidates.push_back({score, candidate});
    }

    sort(movingCandidates.begin(), movingCandidates.end(),
         [](const pair<double, int> &lhs, const pair<double, int> &rhs) {
             if (fabs(lhs.first - rhs.first) > 1e-12)
             {
                 return lhs.first > rhs.first;
             }
             return lhs.second < rhs.second;
         });
    int movingRank = 0;
    if (_useRankedSelection && _currentStage == SAStage::DIVERSIFICATION)
    {
        movingRank = chooseBiasedRank(static_cast<int>(movingCandidates.size()), _rng);
    }
    else
    {
        uniform_int_distribution<int> rankDist(0, static_cast<int>(movingCandidates.size()) - 1);
        movingRank = rankDist(_rng);
    }
    const int moving = movingCandidates[movingRank].second;
    deleteNodeFromTree(moving);

    if (_rootIdx == NIL)
    {
        _rootIdx = moving;
        _nodes[moving].parent = NIL;
        return true;
    }

    struct InsertCandidate
    {
        double score;
        int parent;
        bool asLeftChild;
    };

    vector<InsertCandidate> rankedTargets;
    rankedTargets.reserve(sampleCount * 2);
    for (int i = 0; i < sampleCount; ++i)
    {
        const int parentIdx = nodeDist(_rng);
        if (parentIdx == moving)
        {
            continue;
        }
        rankedTargets.push_back({scoreAttachOption(moving, parentIdx, true), parentIdx, true});
        rankedTargets.push_back({scoreAttachOption(moving, parentIdx, false), parentIdx, false});
    }

    if (rankedTargets.empty())
    {
        if (_rootIdx == NIL)
        {
            _rootIdx = moving;
        }
        return true;
    }

    sort(rankedTargets.begin(), rankedTargets.end(),
         [](const InsertCandidate &lhs, const InsertCandidate &rhs) {
             if (fabs(lhs.score - rhs.score) > 1e-12)
             {
                 return lhs.score > rhs.score;
             }
             if (lhs.parent != rhs.parent)
             {
                 return lhs.parent < rhs.parent;
             }
             return static_cast<int>(lhs.asLeftChild) < static_cast<int>(rhs.asLeftChild);
         });
    int targetRank = 0;
    if (_useRankedSelection && _currentStage == SAStage::DIVERSIFICATION)
    {
        targetRank = chooseBiasedRank(static_cast<int>(rankedTargets.size()), _rng);
    }
    else
    {
        uniform_int_distribution<int> rankDist(0, static_cast<int>(rankedTargets.size()) - 1);
        targetRank = rankDist(_rng);
    }
    insertNode(rankedTargets[targetRank].parent, moving, rankedTargets[targetRank].asLeftChild);
    return true;
}

/**
 * Move one subtree to a new legal attachment point.
 * Refinement uses this version to make more controlled topology changes than
 * the broader delete-and-reinsert move.
 */
bool Floorplanner::perturbMoveSubtree()
{
    if (_currentStage == SAStage::BROAD_EXPLORATION || _currentStage == SAStage::LEGALIZATION)
    {
        return perturbDeleteInsert();
    }

    if (_nodes.size() < 2)
    {
        return false;
    }

    const int sampleCount = min<int>(24, static_cast<int>(_nodes.size()) - 1);
    uniform_int_distribution<int> nodeDist(1, static_cast<int>(_nodes.size()) - 1);

    vector<pair<double, int>> movingCandidates;
    movingCandidates.reserve(sampleCount);
    for (int i = 0; i < sampleCount; ++i)
    {
        const int candidate = nodeDist(_rng);
        const double leafBonus = _nodes[candidate].isLeaf() ? 1.5 : 0.0;
        const double depthBonus = 0.35 * nodeDepth(candidate);
        const double subtreePenalty = 0.30 * static_cast<double>(estimateSubtreeSize(candidate));
        const double score = scoreNodeCongestion(candidate) + leafBonus + depthBonus - subtreePenalty;
        movingCandidates.push_back({score, candidate});
    }

    sort(movingCandidates.begin(), movingCandidates.end(),
         [](const pair<double, int> &lhs, const pair<double, int> &rhs) {
             if (fabs(lhs.first - rhs.first) > 1e-12)
             {
                 return lhs.first > rhs.first;
             }
             return lhs.second < rhs.second;
         });
    int movingRank = 0;
    if (_useRankedSelection && _currentStage == SAStage::REFINEMENT)
    {
        movingRank = chooseBiasedRank(static_cast<int>(movingCandidates.size()), _rng);
    }
    else
    {
        uniform_int_distribution<int> rankDist(0, static_cast<int>(movingCandidates.size()) - 1);
        movingRank = rankDist(_rng);
    }
    const int moving = movingCandidates[movingRank].second;
    if (moving == NIL)
    {
        return false;
    }

    const int oldParent = _nodes[moving].parent;
    if (oldParent == NIL)
    {
        return false;
    }

    const bool wasLeftChild = (_nodes[oldParent].left == moving);
    if (wasLeftChild)
    {
        _nodes[oldParent].left = NIL;
    }
    else
    {
        _nodes[oldParent].right = NIL;
    }
    _nodes[moving].parent = NIL;

    vector<pair<int, bool>> attachOptions;
    attachOptions.reserve(_nodes.size() * 2);
    for (size_t i = 0; i < _nodes.size(); ++i)
    {
        const int parentIdx = static_cast<int>(i);
        if (parentIdx == moving || isAncestor(moving, parentIdx))
        {
            continue;
        }
        if (_nodes[parentIdx].left == NIL && !(parentIdx == oldParent && wasLeftChild))
        {
            attachOptions.push_back({parentIdx, true});
        }
        if (_nodes[parentIdx].right == NIL && !(parentIdx == oldParent && !wasLeftChild))
        {
            attachOptions.push_back({parentIdx, false});
        }
    }

    if (attachOptions.empty())
    {
        if (wasLeftChild)
        {
            _nodes[oldParent].left = moving;
        }
        else
        {
            _nodes[oldParent].right = moving;
        }
        _nodes[moving].parent = oldParent;
        return false;
    }

    vector<pair<double, pair<int, bool>>> rankedTargets;
    rankedTargets.reserve(attachOptions.size());
    for (const pair<int, bool> &target : attachOptions)
    {
        const double score = scoreAttachOption(moving, target.first, target.second);
        rankedTargets.push_back({score, target});
    }

    sort(rankedTargets.begin(), rankedTargets.end(),
         [](const pair<double, pair<int, bool>> &lhs, const pair<double, pair<int, bool>> &rhs) {
             if (fabs(lhs.first - rhs.first) > 1e-12)
             {
                 return lhs.first > rhs.first;
             }
             if (lhs.second.first != rhs.second.first)
             {
                 return lhs.second.first < rhs.second.first;
             }
             return static_cast<int>(lhs.second.second) < static_cast<int>(rhs.second.second);
         });
    int targetRank = 0;
    if (_useRankedSelection && _currentStage == SAStage::REFINEMENT)
    {
        targetRank = chooseBiasedRank(static_cast<int>(rankedTargets.size()), _rng);
    }
    else
    {
        uniform_int_distribution<int> rankDist(0, static_cast<int>(rankedTargets.size()) - 1);
        targetRank = rankDist(_rng);
    }
    const pair<int, bool> bestTarget = rankedTargets[targetRank].second;
    if (bestTarget.second)
    {
        _nodes[bestTarget.first].left = moving;
    }
    else
    {
        _nodes[bestTarget.first].right = moving;
    }
    _nodes[moving].parent = bestTarget.first;
    return true;
}

/**
 * Run a short greedy legalization pass after SA if needed.
 * The pass samples several pressure-driven edits and keeps the candidate that
 * reduces overflow the most while preserving the best fallback search cost.
 */
void Floorplanner::greedyLegalize(double alpha, int attempts)
{
    for (int attempt = 0; attempt < attempts && !isFeasible(); ++attempt)
    {
        SolutionState base = captureSolution();
        const double baseOverflow = calcOverflowRatio(base.overflowWidth, base.overflowHeight);
        SolutionState bestCandidate;
        double bestOverflow = baseOverflow;
        double bestCost = base.searchCost;

        const double overflowX =
            static_cast<double>(_overflowWidth) / max(static_cast<double>(_outlineWidth), 1.0);
        const double overflowY =
            static_cast<double>(_overflowHeight) / max(static_cast<double>(_outlineHeight), 1.0);

        const int trials = 8;
        for (int trial = 0; trial < trials; ++trial)
        {
            restoreSolution(base);

            bool changed = false;
            if (overflowX >= overflowY)
            {
                changed = (trial % 4 == 0) ? perturbMoveSubtree()
                         : (trial % 4 == 1) ? perturbSwapBlocks()
                         : (trial % 4 == 2) ? perturbRotate()
                                            : perturbMoveSubtree();
            }
            else
            {
                changed = (trial % 4 == 0) ? perturbRotate()
                         : (trial % 4 == 1) ? perturbMoveSubtree()
                         : (trial % 4 == 2) ? perturbSwapBlocks()
                                            : perturbRotate();
            }

            if (!changed)
            {
                continue;
            }

            updateCurrentSolution(alpha);
            const double candidateOverflow = calcOverflowRatio(_overflowWidth, _overflowHeight);
            if (candidateOverflow + 1e-12 < bestOverflow ||
                (fabs(candidateOverflow - bestOverflow) <= 1e-12 && _searchCost < bestCost))
            {
                bestOverflow = candidateOverflow;
                bestCost = _searchCost;
                bestCandidate = captureSolution();
            }
        }

        if (bestCandidate.valid)
        {
            restoreSolution(bestCandidate);
        }
        else
        {
            restoreSolution(base);
            break;
        }
    }
}
