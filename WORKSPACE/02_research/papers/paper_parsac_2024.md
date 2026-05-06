# Paper: PARSAC (2024)

原文：
- [PARSAC.pdf](/mnt/d/Bili/PD/FINAL/Reference/PARSAC.pdf)

## 這篇在解什麼

問題設定是現代 SoC / subsystem floorplanning：
- 不只 area / wirelength
- 還有 hard constraints：
  - fixed-outline
  - boundary
  - grouping
  - pre-placement

作者的主張很明確：
- 只把 hard constraints 放進 cost penalty，不夠
- 會得到很多非法或低品質解

## 核心方法：CA-SA

Constraints-Aware Simulated Annealing (CA-SA)。

重點不是「又一個 SA」，
而是：
- move set 被改造成知道 constraints
- 必要時會做 constraints-fixing move
- pre-placed blocks 用 anchored blocks 解，不靠 penalty 猜位置

## 對你最重要的三件事

### 1. constraints-fixing move

論文明講：
- 標準 SA move（swap / move / aspect ratio perturb）
- 加上一小機率的 constraints-fixing move
- 這類 move 不看 cost，直接接受，只為了修 boundary violations

這非常適合你現在的需求。

因為你有：
- outline legality
- 之後若接 E2026 還會有 edge/block/channel 類型約束

建議直接轉成：
- `legality-repair move`
- `congestion-repair move`

### 2. anchored blocks

對 pre-placement，論文不是加距離 penalty，
而是讓被約束 block 在 decode 時 anchored 到固定位置，
children 仍依 B*-tree 關係展開。

這對 E2026 的 edge blocks 很有價值。
可轉成：
- edge block 不完全服從一般 B*-tree 相對位置
- decode 時強制投影到合法邊界錨點

### 3. parallel search + Pareto front

PARSAC 用大量獨立 worker 找 Pareto front。

對你現在的直接啟示不是一定要平行化，
而是：
- multi-start 本來就值得保留
- 不必只留單一 best state
- 可以保留一組：
  - 最小 area
  - 最小 wire
  - 最少 overflow
  - 最佳綜合 cost

## 和你現有 repo 的關係

`BTREE_SA` 現在已經有：
- stage-based SA
- overflow penalty
- feasible lock
- multi-start

但還缺：
- 真正的 constraints-fixing move
- anchored block decode
- 專門的 congestion-aware repair

## 我對這篇的結論

如果你問「B*-tree 可不可以結合 SA+其他做法為底？」

答案是可以，而且我會建議：
- 表示法：B*-tree
- 搜尋主體：SA
- 結構補強：PARSAC 式 constraints-aware move
- 壅塞補強：RUDY/whitespace/inflation 風格 repair

這比單純「B*-tree + 原生 SA」強很多，也更接近你要的方向。

