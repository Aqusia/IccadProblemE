# Current SA Baseline

對照來源：
- [BTREE_SA/Reference/SA_method.txt](/mnt/d/Bili/PD/FINAL/BTREE_SA/Reference/SA_method.txt)
- [floorplanner.h](/mnt/d/Bili/PD/FINAL/BTREE_SA/src/floorplanner.h:1)
- [sa.cpp](/mnt/d/Bili/PD/FINAL/BTREE_SA/src/sa.cpp:1)

## 現況摘要

這個 repo 不是傳統單段 SA。
目前已經是：
- multi-start
- multi-stage
- normalized search cost
- overflow-aware
- feasible-lock
的 SA baseline。

## 目前已有的 stage

- `BROAD_EXPLORATION`
- `LEGALIZATION`
- `REFINEMENT`
- `DIVERSIFICATION`

## 目前已有的好東西

### 1. Multiplicative overflow penalty
- 不是單純線性 overflow 罰
- 對雙維超出更敏感

### 2. Dynamic normalization
- `areaNorm / wireNorm` 從 pool 估
- 比硬寫常數合理

### 3. Multi-start outer runs
- 同一 solver run 內會跑多個 seeds
- 這對 SA 很重要

### 4. Feasible ranking
- 合法解會另外比較
- 不會和非法解混在一起

## 目前最大的缺口

### 1. congestion 還不是 route-aware
目前 `scoreNodeCongestion()` 比較接近：
- overflow / aspect proxy
- 幾何位置壓力

不是：
- routing density
- channel bottleneck
- path resource pressure

### 2. 沒有真正的 constraints-fixing move
目前有 greedy legalization，
但還沒有像 PARSAC 那種：
- 小機率強制修違規 move

### 3. 沒有 inflation / halo model
目前還沒有把壅塞區轉成：
- effective occupancy
- spacing pressure

## 結論

這個 baseline 已經比「從零寫 B*-tree SA」前進很多。

最值得做的不是推翻它，
而是：
- 把 cost 擴成 route-aware
- 把 move 擴成 constraints-aware / congestion-aware
- 把 legalization 擴成 targeted repair

