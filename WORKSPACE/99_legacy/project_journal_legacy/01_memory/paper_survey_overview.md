# Paper Survey Overview

目的：
- 統整目前資料夾中的參考 paper
- 快速判斷哪些想法可直接用在 `E_2026 Early Floorplanning with Global Route`

## 優先度總覽

### 第一優先：最直接可用
- `505388.505402.pdf`
  - routability-driven floorplanner + congestion estimation + buffer planning
- `PARSAC.pdf`
  - constraints-aware simulated annealing，特別適合硬限制 floorplanning
- `Routability-driven_placement_and_white_space_allocation.pdf`
  - 以 congestion map 驅動留白與 re-distribution
- `NTUplace4h_A_Novel_Routability-Driven_Placement_Algorithm_for_Hierarchical_Mixed-Size_Circuit_Designs.pdf`
  - 對 narrow channel、pin density、overflow、net congestion 都有明確處理觀點

### 第二優先：routing engine / congestion 模型
- `FastRoute_4.0_Global_router_with_efficient_via_minimization.pdf`
- `NCTU-GR_2.0_Multithreaded_Collision-Aware_Global_Routing_With_Bounded-Length_Maze_Routing.pdf`
- `775832.775842.pdf`
- `A_SimPLR_method_for_routability-driven_placement.pdf`

### 第三優先：延伸方法 / 特定場景
- `Routability-Driven_TSV-Aware_Floorplanning_Methodology_for_Fixed-Outline_3-D_ICs.pdf`
- `b27805281.pdf`
- `A_review_on_VLSI_floorplanning_optimization_using_metaheuristic_algorithms.pdf`

### 第四優先：可參考但先保留
- `Reinforcement-Learning-For-Automated-Chip-Floorplanning-And-Routing-Optimization.pdf`

## 各篇 paper 可用價值摘要

### 1. Routability Driven Floorplanner with Buffer Block Planning
最有價值的點：
- 把 area 與 congestion 拆成 two-phase SA
- 用 probabilistic analysis 做 congestion estimation
- 不只看 routing，還把 buffer / resource feasibility 一起建模

對本題可借用：
- 先合法 / 粗優化，再針對 routing 壓力微調
- 把「通道資源」視為有限容量，而不是 route 後再補救

### 2. Improved Global Routing through Congestion Estimation
最有價值的點：
- rip-up and reroute 不只靠當前 congestion，而是用放大後的 congestion estimate 逼演算法避開熱點

對本題可借用：
- 早期 route assignment 若發現某些 channel 常爆，可對其成本做 amplification
- 避免每輪都走看似最短但會累積壅塞的路

### 3. SimPLR
最有價值的點：
- simultaneous place-and-route
- lookahead routing
- 同時把擁塞區擴散、非擁塞區收緊

對本題可借用：
- route-aware placement，而不是先 placement 後 route
- 可以用簡化 lookahead router 當 inner loop evaluator

### 4. White Space Allocation
最有價值的點：
- 不只是搬 cell，而是依 congestion map 分配 white space

對本題可借用：
- 把 channel 視為顯式 white space 資源
- 不是 uniform 地留白，而是依 net demand 分佈決定哪裡要寬

### 5. NTUplace4h
最有價值的點：
- 把 routability 問題拆成多個可優化子項：
  - narrow channel
  - pin density
  - routing overflow
  - net congestion

對本題可借用：
- 為 cost function 拆成多個 penalty term
- 讓 early floorplanning 不只壓 HPWL

### 6. FastRoute 4.0
最有價值的點：
- routing flow 全程考慮 via，不只是 maze cost 裡加 via cost
- via-aware Steiner tree generation
- 3-bend routing

對本題可借用：
- 雖然本題未必要求 via，但可借「routing flow 全程一貫考慮 secondary objective」這個設計觀念
- 可用 2-bend / 3-bend pattern route 當快速候選路徑生成器

### 7. NCTU-GR 2.0
最有價值的點：
- bounded-length maze routing
- collision-aware multithreading
- RSMT-aware routing guidance

對本題可借用：
- 若之後 route candidate 太多，可限制搜尋長度，控制 runtime
- path search 可先看 bounded detour，而不是無界迷宮

### 8. PARSAC
最有價值的點：
- constraints-aware SA
- 不把 hard constraints 只當 cost penalty，而是讓 search move / legal construction 本身知道限制
- parallel SA

對本題可借用：
- edge block 邊界位置、outline、hard block legality，最好是 move generator 直接保證
- 不要讓 SA 大量浪費在非法解

### 9. TSV-aware 3D Floorplanning
最有價值的點：
- fixed-outline 下同時處理 wirelength 與 routability
- deterministic algorithm 比純 SA 更有效率

對本題可借用：
- 本題也有 fixed-outline 味道
- 可考慮 deterministic initialization + local search refinement

### 10. Analog / Mixed-Signal Routability-Driven Floorplanning Thesis
最有價值的點：
- sequence pair / polish expression 兩套表示法
- 以 congestion probability 驅動 module expansion
- mixed constraints 共存

對本題可借用：
- 若要做較完整 floorplanner，表示法選擇很重要
- soft block 面積擴張可由 congestion / FT 壓力驅動

### 11. Metaheuristic Review
最有價值的點：
- 快速比較 SA / PSO / ACO
- floorplan representation 影響 search space 與 move complexity

對本題可借用：
- 若只求可做可解，SA 是最穩妥 baseline
- 若之後要擴展，表示法比演算法名稱更關鍵

### 12. RL-EDA
最有價值的點：
- 多目標 reward 設計
- placement / routing 視為 sequential decision making

對本題可借用：
- 主要價值在 reward decomposition 思路
- 不建議作為這題第一版方法

## 對本題最值得落地的 8 個技術方向

1. `constraints-aware initialization`
2. `two-stage or multi-stage optimization`
3. `route-aware cost evaluation`
4. `congestion-driven channel sizing`
5. `feedthrough-aware soft block expansion`
6. `bounded detour path search`
7. `white-space / channel redistribution`
8. `multi-term objective decomposition`

## 建議實作順序

1. 先做合法 floorplan 產生器
2. 加 block-level routing demand model
3. 加 channel demand -> width 反推
4. 加 feedthrough assignment / expansion
5. 加 cost refinement loop
6. 最後再考慮更強的 search / parallelization

