# Reusable Techniques

這份不是逐篇摘要，而是把 paper 中可直接轉成 solver 設計的點重組。

## A. 問題拆解方式

### 1. 分階段最佳化
來源：
- `505388.505402.pdf`
- `PARSAC.pdf`
- `Routability-Driven_TSV-Aware_Floorplanning_Methodology_for_Fixed-Outline_3-D_ICs.pdf`

可用做法：
- Phase 1：先求合法且面積不離譜的初始 floorplan
- Phase 2：加入 route / channel / FT penalty
- Phase 3：局部修正 narrow channel、overflow、edge constraint

為什麼重要：
- 本題限制多，若一開始就把所有限制丟進同一個 search，容易收斂很慢

### 2. 同步 place-and-route 評估
來源：
- `A_SimPLR_method_for_routability-driven_placement.pdf`

可用做法：
- 每次 block 移動後，不只更新 HPWL
- 也同步跑一個快速 route estimator
- 用 route estimator 回饋 block placement

## B. Floorplan 表示法

### 1. Sequence Pair
來源：
- `b27805281.pdf`
- `A_review_on_VLSI_floorplanning_optimization_using_metaheuristic_algorithms.pdf`

優點：
- 適合 non-slicing
- 表示能力強

缺點：
- decode 與 move 評估相對重

### 2. Polish Expression / Slicing Tree
來源：
- `b27805281.pdf`

優點：
- 結構化、操作單純

缺點：
- 表示空間受 slicing 限制

### 3. 實務建議
若目標是快速做 contest baseline：
- 不一定需要完整 SP/PE
- 可以先用 rectangle packing + local swap / resize / shift

若目標是較完整研究版：
- SP 是較有彈性的選擇

## C. Cost Function 拆解

### 建議 cost 項目
- `wirelength_term`
- `outline_penalty`
- `overlap_penalty`
- `channel_overflow_penalty`
- `feedthrough_overflow_penalty`
- `soft_block_expansion_penalty`
- `edge_block_violation_penalty`
- `narrow_channel_penalty`
- `congestion_hotspot_penalty`

### 重要觀念
來源：
- `NTUplace4h...pdf`
- `Routability-driven_placement_and_white_space_allocation.pdf`

不要把 routability 壓成單一數字。
至少拆成：
- local channel congestion
- global overflow
- pin / interface concentration
- 由 FT 引入的 block enlargement

## D. Congestion Estimation

### 1. 機率式 congestion estimation
來源：
- `505388.505402.pdf`

可用做法：
- 對一個 net，不只考慮單一路徑
- 對可行 route 候選分配機率或 cost
- 估每條 channel 被使用的期望 demand

用途：
- 比單一最短路更穩
- 適合 early stage 粗估

### 2. amplified congestion map
來源：
- `775832.775842.pdf`

可用做法：
- 若某些 channel 在多輪反覆過載
- 對其成本額外放大
- 逼後續 net 避開熱點

### 3. lookahead routing
來源：
- `A_SimPLR_method_for_routability-driven_placement.pdf`

可用做法：
- 在 placement loop 中用輕量 router 預看壅塞
- 不用等最後一輪才知道 placement 不可 route

## E. Routing Candidate 生成

### 1. bounded detour search
來源：
- `NCTU-GR_2.0...pdf`

可用做法：
- 先生成最短 Manhattan path
- 再允許有限長度 detour
- 超過界限就不搜

效果：
- 大幅控制 runtime
- 又保留必要繞路能力

### 2. pattern routing as first pass
來源：
- `FastRoute_4.0...pdf`
- `NCTU-GR_2.0...pdf`

可用做法：
- 先 1-bend / 2-bend / 3-bend 候選
- 候選都失敗再進 maze-like search

### 3. RSMT / Steiner guidance
來源：
- `FastRoute_4.0...pdf`
- `NCTU-GR_2.0...pdf`

本題可簡化成：
- 先用 block center / port side 建較合理的主幹方向
- 再分派到 channel graph

## F. White Space / Channel 管理

### 1. congestion-driven whitespace allocation
來源：
- `Routability-driven_placement_and_white_space_allocation.pdf`

可用做法：
- 不平均留 channel
- 根據 connection demand 在局部撐寬
- 冷區則收緊

### 2. narrow channel handling
來源：
- `NTUplace4h...pdf`

可用做法：
- 對過窄 channel 加強 penalty
- 優先移開會造成 bottleneck 的 block pair

## G. Feedthrough 建模

### 1. soft block expansion
來源：
- `b27805281.pdf`
- `505388.505402.pdf`

可用做法：
- 當 soft block 承接 FT demand 時，不把 FT 視為免費
- 依 conversion efficiency / demand 增加 block area

### 2. FT vs channel tradeoff
來源：
- `E_20260414.pdf`

可用做法：
- 對每個大需求 connection 比較兩種代價：
  - 全走 channel 需要多少 channel width / area
  - 部分轉 FT 需要多少 soft block enlargement

## H. 硬限制處理

### constraints-aware move generation
來源：
- `PARSAC.pdf`

核心觀念：
- 不要只靠 cost 懲罰非法解
- move 本身就要盡量維持 legality

本題應用：
- edge block 位置限制
- hard block 不可變形
- soft block aspect ratio
- outline 內放置

## I. 平行化與效能

### 1. multithread routing
來源：
- `NCTU-GR_2.0...pdf`

可用做法：
- 若後續 Python baseline 太慢，可把 route evaluation 平行化

### 2. parallel SA
來源：
- `PARSAC.pdf`

可用做法：
- 多起點 search
- 保留 Pareto 候選解

## J. RL 的正確定位

來源：
- `Reinforcement-Learning-For-Automated-Chip-Floorplanning-And-Routing-Optimization.pdf`

對本題真正可用的，不是直接上 RL，而是：
- reward 項拆解觀念
- sequential decision 表述

不建議第一版就做 RL，原因：
- contest 先需要穩定、可 debug、可控 runtime 的 solver

