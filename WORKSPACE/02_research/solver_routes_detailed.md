# Solver Routes Detailed

目的：
- 把目前可行的 solver 方案拆成多條路線
- 每條路線都說清楚：
  - 核心想法
  - 需要哪些資料結構
  - 演算法流程
  - 優缺點
  - 何時該選

## Route A: 現有 `BTREE_SA` 漸進升級版

定位：
- 最推薦
- 重用率最高

核心想法：
- 保留現有 `B*-tree + multi-stage SA`
- 增加：
  - `constraints-fixing move`
  - `fast congestion evaluator`
  - `virtual halo / spacing pressure`
  - `top-k hotspot repair`

優點：
- 最貼近現在 repo
- 能很快開始做 code

缺點：
- 需要小心避免把 cost 堆太肥

## Route B: Constraints-Aware SA 正規版

定位：
- 把 Route A 再系統化

核心想法：
- 以 PARSAC 為中心
- legality / congestion repair move 明確獨立
- SA 不只是 cost optimization，也是 constraint management

適合：
- baseline 跑穩之後升級

## Route C: B*-tree + dual evaluator

定位：
- 想同時兼顧速度與較像 routing 的回饋

核心想法：
- `fast_eval`：RUDY / bbox density / bottleneck proxy
- `accurate_eval`：candidate path / channel assignment

優點：
- 比只用單一 proxy 更穩

缺點：
- 需要做兩套 evaluator

## Route D: 如果要接 E2026 完整題目

定位：
- 從目前 hard-macro floorplanner 擴成 early floorplanning with global route

要新增：
- edge/hard/soft block type
- channel graph
- FT model
- top-wire model
- route-resource penalties

## 最推薦採用方式

### 現在
- `Route A + Route C`

### 下一階段
- 把它整理成 `Route B`

### 真正接 E2026
- 再接 `Route D`

