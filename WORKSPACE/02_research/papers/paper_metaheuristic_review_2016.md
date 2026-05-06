# Paper: Review on VLSI Floorplanning Optimization using Metaheuristics (2016)

原文：
- [A_review_on_VLSI_floorplanning_optimization_using_metaheuristic_algorithms.pdf](/mnt/d/Bili/PD/FINAL/REFERENCE/A_review_on_VLSI_floorplanning_optimization_using_metaheuristic_algorithms.pdf)

## 這篇在解什麼

這是一篇 survey。
它不是提出新演算法，而是整理：
- floorplan representations
- SA / PSO / ACO 等 metaheuristics
- hard / soft module 問題設定

## 對你真正有用的點

### 1. representation 會影響 search quality，但不是唯一關鍵
- B*-tree
- Sequence Pair
- O-tree
- CBL / TCG 等

這篇的實際訊息是：
- 表示法要選一個實作成熟、move 自然、decode 快的
- 後面該把心力放在 cost / repair / flow

### 2. SA 仍然是很合理的 baseline
- search 空間巨大
- 問題本質是 NP-hard
- SA 對混合目標與複雜限制仍然實用

### 3. multi-objective cost 是常態
- area
- wirelength
- aspect ratio
- legality
- 其他設計指標

這點對你很重要，因為你現在也不是只看 area/wire。

## 對本專案的結論

這篇不提供細節招式，
但它支撐一個方向判斷：

- 保留 `B*-tree + SA` 是合理的
- 真正該做創新的地方是：
  - move set
  - congestion estimator
  - legality / routability repair
  - multi-term objective

## 我對這篇的結論

這篇屬於「策略確認」型參考：
- 用來證明你現在的主線不怪
- 但不應該成為主要技術來源
