# Paper Survey Overview

目的：
- 統整目前資料夾中的參考 paper
- 快速判斷哪些想法可直接用在 `E_2026 Early Floorplanning with Global Route`
- 幫 `B*-tree + SA` 這條主線找最相關的支撐方法

## 優先度總覽

### 第一優先：最直接可用
- `Routability_Driven_Floorplanner_with_Buffer_Block_Planning.pdf`
  - routability-driven floorplanner + congestion estimation + buffer/resource planning
- `PARSAC.pdf`
  - constraints-aware simulated annealing，特別適合硬限制 floorplanning
- `Routability-driven_placement_and_white_space_allocation.pdf`
  - 以 congestion map 驅動留白與 re-distribution
- `B*-Trees` / `Are Floorplan Representations Important`
  - 用來確認表示法與現有 repo 的主線是否合理

### 第二優先：routing engine / congestion 模型
- `FastRoute_4.0_Global_router_with_efficient_via_minimization.pdf`
- `NCTU-GR_2.0_Multithreaded_Collision-Aware_Global_Routing_With_Bounded-Length_Maze_Routing.pdf`
- `Improved_Global_Routing_through_Congestion_Estimation.pdf`
- `A_SimPLR_method_for_routability-driven_placement.pdf`

### 第三優先：延伸方法 / 特定場景
- `NTUplace4h_A_Novel_Routability-Driven_Placement_Algorithm_for_Hierarchical_Mixed-Size_Circuit_Designs.pdf`
- `Routability_Driven_Floorplanning_of_Analog_and_Mixed_Signal_Circuits_Thesis.pdf`
- `A_review_on_VLSI_floorplanning_optimization_using_metaheuristic_algorithms.pdf`
- `Routability-Driven_TSV-Aware_Floorplanning_Methodology_for_Fixed-Outline_3-D_ICs.pdf`

### 第四優先：可參考但先保留
- `Reinforcement-Learning-For-Automated-Chip-Floorplanning-And-Routing-Optimization.pdf`

## 對目前主線最重要的 6 篇

### 1. B*-Trees
作用：
- 確認 `BTREE_SA` 這條表示法主線合理
- 提供更強的 move 思路，特別是 delete/insert、soft module 處理

### 2. Are Floorplan Representations Important
作用：
- 幫你做決策：不要把時間浪費在改表示法宗教戰
- 現有 repo 用 B*-tree 是合理的

### 3. PARSAC
作用：
- 提供 `constraints-aware move`
- 提供 anchored/pre-placed 的想法
- 非常適合把現有 SA 升級成更懂約束的版本

### 4. Routability Driven Floorplanner with Buffer Block Planning
作用：
- 提供 `two-phase SA + routability-aware evaluator`
- 可以把 buffer/resource feasibility 轉譯成 channel/FT feasibility

### 5. White Space Allocation
作用：
- 告訴你擁擠區不是只靠 penalty，要做 white-space redistribution
- 對目前 hard-macro repo，可轉成 `virtual inflation / spacing / bottleneck expansion`

### 6. RUDY / OpenROAD
作用：
- 提供 fast congestion estimator
- 適合放進 SA 內層

## 各篇 paper 可用價值摘要

### B*-Trees
最有價值的點：
- B*-tree 對 non-slicing floorplan、SA move、pre-placed/soft module 都友善
- decode 快、操作直覺

對本專案可借用：
- 保留 B*-tree
- 補強 delete+reinsert、constraint-aware、congestion-aware move

### Are Floorplan Representations Important
最有價值的點：
- representation 影響存在，但通常不如 objective / flow / legalization 重要

對本專案可借用：
- 繼續沿用 B*-tree
- 把時間投到 congestion / penalty / repair

### PARSAC
最有價值的點：
- constraints-fixing moves
- anchored pre-placed blocks
- 不只靠 penalty 懲罰非法解

對本專案可借用：
- 在 `BTREE_SA` 補 `legality-fixing move`
- 若接 E2026 edge blocks，可考慮 anchored decode

### Routability Driven Floorplanner with Buffer Block Planning
最有價值的點：
- two-phase SA
- probabilistic congestion estimation
- routability resource 不是最後才看

對本專案可借用：
- Stage 1 法律+粗成本
- Stage 2 加 congestion / route proxy

### White Space Allocation
最有價值的點：
- congestion-driven whitespace redistribution
- bottleneck area 不應平均分配空間

對本專案可借用：
- top-k hot region repair
- virtual halo / spacing pressure

### RUDY / OpenROAD
最有價值的點：
- ultra-fast congestion estimate
- inflation / RC-driven stop rule

對本專案可借用：
- fast inner-loop evaluator
- halo / virtual inflation 機制

## 對目前 repo 最值得落地的 8 個技術方向

1. `constraints-aware move generation`
2. `two-stage or multi-stage routability-aware SA`
3. `fast congestion evaluator`
4. `top-k hotspot targeted repair`
5. `virtual inflation / halo`
6. `spacing / bottleneck expansion`
7. `bounded detour / route proxy`
8. `multi-term objective decomposition`

