# Paper: FastRoute 4.0

原文：
- [FastRoute_4.0_Global_router_with_efficient_via_minimization.pdf](/mnt/d/Bili/PD/FINAL/REFERENCE/FastRoute_4.0_Global_router_with_efficient_via_minimization.pdf)

## 這篇在解什麼

重點不是 floorplanning，
而是 global routing 裡如何同時顧：
- overflow
- wirelength
- via count

作者主張：
- via 不是 routing 後段才補的 secondary metric
- 要在 Steiner tree、pattern routing、layer assignment 全流程一起處理

## 核心方法

### 1. via-aware Steiner tree generation
- 在樹生成時就偏好較少 via 的 topology

### 2. 3-bend routing
- 控制 routing pattern 複雜度
- 在 quality 和 runtime 間取平衡

### 3. ordered layer assignment
- layer assignment 不是獨立最後一步
- 也會影響 via 數量與路徑品質

## 對你現在的價值

雖然你不是要做 router，
但這篇提醒兩件事：

### 1. secondary metric 要提早進 objective
- 對你來說可類比成：
  - top-wire usage
  - feedthrough pressure
  - channel overflow

### 2. route proxy 不只看平面擁擠
- 還要考慮 path shape 複雜度
- 避免用太樂觀的 shortest-bbox 假設

## 可直接借用

- 如果 E2026 最後要估 top-wire / FT 成本
- 可以做簡化的 path-shape penalty
- 對多彎折、強迫繞路的 net 額外加 cost

## 我對這篇的結論

這篇不直接改你的 `B*-tree + SA` 架構，
但它很適合幫你補：
- route quality proxy
- top-wire / detour / bend 相關成本項
