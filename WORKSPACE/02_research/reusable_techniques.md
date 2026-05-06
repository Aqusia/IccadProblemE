# Reusable Techniques

這份不是逐篇摘要，而是把 paper 中可直接轉成 solver 設計的點重組。

## A. 問題拆解方式

### 1. 分階段最佳化
來源：
- `Routability_Driven_Floorplanner_with_Buffer_Block_Planning.pdf`
- `PARSAC.pdf`
- `Routability-Driven_TSV-Aware_Floorplanning_Methodology_for_Fixed-Outline_3-D_ICs.pdf`

可用做法：
- Phase 1：先求合法且面積不離譜的初始 floorplan
- Phase 2：加入 route / channel / FT / congestion penalty
- Phase 3：局部修正 narrow channel、overflow、edge constraint、hotspot

### 2. 同步 place-and-route 評估
來源：
- `A_SimPLR_method_for_routability-driven_placement.pdf`

可用做法：
- 每次 block 移動後，不只更新 HPWL
- 也同步跑一個快速 route estimator
- 用 route estimator 回饋 block placement

## B. Floorplan 表示法

### 1. B*-tree
來源：
- `B*-Trees`
- `Are Floorplan Representations Important`

優點：
- 對現有 repo 最相容
- move 操作自然
- decode 較快

缺點：
- routability 不是 representation 自帶的，要另外建

### 2. Sequence Pair
來源：
- `Routability_Driven_Floorplanning_of_Analog_and_Mixed_Signal_Circuits_Thesis.pdf`
- `A_review_on_VLSI_floorplanning_optimization_using_metaheuristic_algorithms.pdf`

優點：
- 表示能力強

缺點：
- decode 與 move 評估相對重

### 3. 實務建議
目前直接選：
- `B*-tree`

真正該補的是：
- constraints-aware move
- congestion model
- repair / spacing / inflation

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
- `spacing_pressure_penalty`
- `virtual_halo_penalty`

### 重要觀念
不要把 routability 壓成單一數字。
至少拆成：
- local congestion
- global overflow
- bottleneck gap
- interface concentration
- halo / spacing 壓力

## D. Congestion Estimation

### 1. 機率式 congestion estimation
來源：
- `Routability_Driven_Floorplanner_with_Buffer_Block_Planning.pdf`

可用做法：
- 對一個 net，不只考慮單一路徑
- 對可行 route 候選分配機率或 cost
- 估每條 channel 被使用的期望 demand

### 2. amplified congestion map
來源：
- `Improved_Global_Routing_through_Congestion_Estimation.pdf`

可用做法：
- 若某些 channel / region 在多輪反覆過載
- 對其成本額外放大

### 3. RUDY-like estimator
來源：
- `RUDY`
- `OpenROAD`

可用做法：
- 每個 net 用 bbox 投影 demand
- 做 ultra-fast heat map

## E. 壅塞區修復

### 1. white-space / spacing redistribution
來源：
- `Routability-driven_placement_and_white_space_allocation.pdf`

可用做法：
- 不平均留白
- 根據 hotspot / bottleneck 做局部擴張

### 2. targeted repair
來源：
- `PARSAC`
- `White Space Allocation`

可用做法：
- 對 top-k hottest nodes/gaps 做局部 move
- 小機率強制修復，不完全依賴 cost 接受

### 3. virtual inflation / halo
來源：
- `OpenROAD`
- 白空間分配類方法

可用做法：
- 對 hard macro 不改真實尺寸
- 在 search 中使用 effective halo

## F. 硬限制處理

### constraints-aware move generation
來源：
- `PARSAC.pdf`

核心觀念：
- 不要只靠 cost 懲罰非法解
- move 本身就要盡量維持 legality

本專案應用：
- outline legality
- 之後若接 E2026，還有 edge block、soft block、FT、channel

## G. 效能策略

### 1. dual evaluator
來源：
- `Interconnect-Driven Floorplanning`
- `RUDY / OpenROAD`

可用做法：
- `fast_eval`: bbox / RUDY / geometry proxy
- `accurate_eval`: candidate path / channel assignment

### 2. parallel multi-start
來源：
- `PARSAC`

可用做法：
- 保留多起點
- 保留 Pareto 候選，不只留單一 best

