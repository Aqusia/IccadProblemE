# Repo Migration Plan

目標：
- 不重寫 `BTREE_SA`
- 直接在現有結構上擴成 routability-aware B*-tree SA

重要澄清：
- 不重寫 `BTREE_SA` 不等於不改 SA move。
- 我們可以保留 repo 架構、資料結構、packing、cost 更新流程，
  但重新設計 perturbation / repair / move selection。

## Step 1: 保持目前主骨架不變

保留：
- B*-tree
- contour packing
- solution snapshot / restore
- SA 骨架
- feasible lock
- normalization

不必強制保留：
- 現在的 perturbation 比例
- 現在的 move 種類
- 現在的 stage schedule 細節

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

## Step 5: 重做 move set + 新增 repair moves

這一步不只是「補兩個 move」。
比較合理的是把目前的 move set 視為 baseline，重新切成：

- generic topology moves
- legality-fixing moves
- congestion-fixing moves
- diversification moves

也就是：
- 可以保留現有 rotate / swap / subtree move / delete-insert
- 但不需要完全照搬
- 可以重新定義觸發條件與權重

### `perturbConstraintFixMove()`
- 專修 outline / boundary 類違規

### `perturbCongestionFixMove()`
- 專修 top-k hotspot / bottleneck

這兩種 move 可在 `perturbRandomMove()` 裡以小機率插入。

如果之後發現有必要，也可以：
- 直接把 `perturbRandomMove()` 的權重機制重寫
- 對不同 stage 用不同 move distribution

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
