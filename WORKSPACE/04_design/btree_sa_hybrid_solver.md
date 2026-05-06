# B*-tree + SA Hybrid Solver

這份是目前最推薦的主線。

## 先回答你的問題

### 1. 用 B*-tree 可不可以？
可以，而且合理。

原因：
- 你已經有現成 `BTREE_SA` repo
- B*-tree 本身就適合 SA
- B*-tree 對 constrained move、delete/insert、local repair 都好接

### 2. 能不能結合 SA + 其他做法？
可以，而且應該這樣做。

我不建議只用「純 SA」。
比較好的架構是：
- `B*-tree`：表示法
- `SA`：全域搜尋主體
- `constraints-fixing moves`：合法性補強
- `fast congestion evaluator`：快速估壅塞
- `targeted repair / inflation / spacing`：處理熱區

### 3. penalty 能不能一起放？
可以，但要拆層次。

不建議把所有東西混成單一 penalty。
至少要分：
- legality penalties
- congestion penalties
- spacing / inflation penalties
- route-resource penalties

### 4. 擁擠區怎麼處理？要不要放大 block？
要處理，但分情況。

對 `hard macro`：
- 不要真的改 block size
- 用：
  - virtual inflation
  - routing halo
  - spacing pressure
  - channel widening

對 `soft block`：
- 可以真的 reshape / inflate

## 最推薦架構

### Layer 1: Topology search
- 用 B*-tree
- 保留：
  - rotate
  - swap
  - subtree move
  - delete + reinsert

### Layer 2: Cost evaluation

`searchCost = baseCost + legalityPenalty + congestionPenalty + spacingPenalty`

其中：

#### Base cost
- normalized area
- normalized wirelength

#### Legality penalty
- fixed-outline overflow
- overlap（若 decode 可能出現）
- boundary/edge violations（之後擴展到 E2026）

#### Congestion penalty
- fast route / density proxy
- hotspot aggregation
- top-k bottleneck emphasis

#### Spacing penalty
- inflation/halo 帶來的 effective area 壓力
- channel shrink penalty

## 我建議的具體流程

### Stage A: Broad exploration
目標：
- 找到幾個 topology basin

做法：
- 用目前 repo 的 stage 1
- legality penalty 保留
- congestion 先用輕量 proxy

### Stage B: Legalization + route-aware refinement
目標：
- 不只合法，也要把擁塞壓下來

做法：
- 繼續用 SA
- 但每個 move 後加：
  - congestion estimate
  - top-k channel/region pressure

### Stage C: Targeted repair
目標：
- 專門修 hottest regions

做法：
- 選 top-k hot blocks / bottleneck gaps
- 限制 move 只在局部做：
  - push apart
  - best reinsert near lower pressure zone
  - rotate for aspect relief
  - halo increase / decrease

### Stage D: Optional diversification
目標：
- 跳出局部極值

做法：
- 保留目前 repo 的 diversification stage
- 但盡量只在已合法前提下做

## 核心補強 1：Constraints-aware move

從 PARSAC 借來的最重要想法：
- 有些 move 不應只看 cost，而是專門修 violation

你現在最該補的兩種：

### legality-fixing move
- 若 overflow 嚴重
- 優先搬移最靠近超出邊界的子樹 / block

### congestion-fixing move
- 若某區過熱
- 優先推開該區塊或改 attach 位置

這兩類 move 可以設成：
- 小機率強制插入
- 成本惡化也可接受

## 核心補強 2：Congestion estimator

### 第一版建議
不要一開始就做完整 global router。

先做：
- `RUDY-like bbox density`
- `block-gap bottleneck score`
- `local boundary pressure`

三個數合成一個 congestion metric。

### 第二版建議
若之後接 E2026：
- 再做 channel graph
- 再做 candidate path assignment
- 再把 FT / top-wire 資源加進來

## 核心補強 3：Inflation / spacing

### Hard-macro repo 的正確作法
不要真的改模組尺寸。
要改成：
- `virtual_inflation[node]`
- `halo[node]`

這個 halo 只影響：
- score
- attach desirability
- local effective occupancy

不影響：
- 真正 output rectangle

### E2026 soft-block 版
若之後擴展到 soft block：
- 才把 inflation 寫成真實寬高 / 面積變化

## 我最後的主張

你這條線最好的版本不是：
- B*-tree vs sequence pair

而是：
- `B*-tree + multi-stage SA + constraints-fixing moves + fast congestion evaluator + targeted inflation/spacing repair`

這個方向和你現有 repo 的重用率最高，也最值得往下做。

