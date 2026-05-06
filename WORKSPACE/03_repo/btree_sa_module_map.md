# BTREE_SA Module Map

## `src/floorplanner.h` / `src/floorplanner.cpp`
- parser
- B*-tree state
- contour packing
- area / HPWL / overflow / search cost 更新
- solution snapshot / restore

## `src/sa.h`
- stage config
- norm stats

## `src/sa.cpp`
- stage schedule
- initial pool
- temperature estimation
- single run / multi-run SA 主流程

## `src/sa_moves.cpp`
- rotate
- swap
- move subtree
- delete-insert
- combo perturb
- greedy legalization
- congestion-like heuristic scoring（目前還只是 geometry/overflow proxy）

## `src/sa_support.cpp`
- feasible solution ranking
- normalization from pool
- Gaussian score
- pool summary

## `tools/`
- `plot_floorplan.py`
- `sweep_sa.py`

## 目前最值得插入新功能的位置

### 1. congestion / inflation
- `scoreNodeCongestion()`
- `scoreAttachOption()`
- `calcSearchCost()`
- `greedyLegalize()`

### 2. new stage logic
- `buildStageConfigs()`
- `runStage()`

### 3. new data model
- `Block` / `Floorplanner` 結構內新增：
  - congestion score
  - halo / virtual inflation
  - route proxy stats

