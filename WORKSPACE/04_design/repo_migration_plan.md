# Repo Migration Plan

目標：
- 不重寫 `BTREE_SA`
- 直接在現有結構上擴成 routability-aware B*-tree SA

## Step 1: 保持目前主骨架不變

保留：
- B*-tree
- contour packing
- solution snapshot / restore
- multi-stage SA
- feasible lock
- normalization

## Step 2: 新增 congestion 資料

在 `Floorplanner` 內新增：
- per-block congestion score
- per-block virtual halo
- coarse heatmap / bins
- top-k hotspot cache

## Step 3: 擴充 score 函式

優先改：
- `scoreNodeCongestion()`
- `scoreRotateCandidate()`
- `scoreSwapPair()`
- `scoreAttachOption()`

讓它們不只看 geometry/overflow，
也看：
- heat
- bottleneck
- net degree pressure

## Step 4: 擴充 search cost

在 `calcSearchCost()` 中加入：
- congestionPenalty
- spacingPenalty

但先保留原本 overflow penalty，不要一次重寫全部。

## Step 5: 新增 repair moves

新增兩種 move：

### `perturbConstraintFixMove()`
- 專修 outline / boundary 類違規

### `perturbCongestionFixMove()`
- 專修 top-k hotspot / bottleneck

這兩種 move 可在 `perturbRandomMove()` 裡以小機率插入。

## Step 6: 新增 fast evaluator

第一版先做：
- bbox density / RUDY-like
- bottleneck gap score

不要先做完整 router。

## Step 7: 若轉向 E2026

再擴充：
- block type：edge/hard/soft
- channel graph
- feedthrough model
- top-wire model

## 目前不建議先做的事

- 改成 sequence pair
- 直接上 RL
- 一開始就做全域 route solver

