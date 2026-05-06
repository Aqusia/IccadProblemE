# All Papers Useful Points

目的：
- 確保 `REFERENCE/` 內每篇主要論文都有被讀過並抽取可用點
- 讓後續設計 `B*-tree + SA` 時，不需要重新翻整包 PDF

## 核心主線相關

### B*-Trees
- 保留 `B*-tree` 當表示法完全合理
- 重點不在換 representation，而在補強 move set、repair、cost
- 可直接借 `delete + reinsert` 類操作

### Are Floorplan Representations Important
- 表示法不是不重要，但通常不是最大瓶頸
- 不要把時間花在 representation 宗教戰

### PARSAC
- 要做 `constraints-aware move`
- 不能只靠 penalty 等非法解自己變好
- anchored / fixed handling 很值得借

## Routability / congestion 主線

### Routability-Driven Floorplanner with Buffer Block Planning
- 最重要的是 `two-phase SA + routability-aware evaluator`
- 特殊 routing 資源應提早進 cost

### White Space Allocation
- hotspot 不該平均分配空間
- 應做 congestion-driven whitespace redistribution

### RUDY / OpenROAD
- 需要 fast congestion proxy
- 適合進 SA 內層

### SimPLR
- congestion 修補要定向作用於 hotspot
- 冷區可以重新壓緊，避免 wirelength 無上限膨脹

### Improved Global Routing through Congestion Estimation
- 對壅塞的懲罰要放大
- repeated hotspot 應有歷史記憶

## Routing engine / route proxy 相關

### FastRoute 4.0
- secondary metric 要早進 objective
- 可轉成 detour / bend / top-wire 類 proxy

### NCTU-GR 2.0
- evaluator 不必處處最精確
- 可以做 bounded approximation，局部精修、全域粗估

## Mixed-size / special-resource / extended cases

### NTUplace4h
- congestion 不只 overflow
- 還要看 narrow channel、pin density、macro porosity

### TSV-aware 3D Floorplanning
- 特殊 routing 資源應提早規劃
- SA 外圍可混 deterministic repair

### Analog and Mixed-Signal Thesis
- block-specific virtual inflation 有直接文獻支持
- 高 congestion probability 區可給更大 spacing pressure

## 方向確認 / future work

### Metaheuristic Review
- `B*-tree + SA` 是合理基底
- 創新應放在 flow、cost、repair

### RL-EDA
- 適合借 reward decomposition / adaptive policy 的想法
- 不適合當這次專案主線

## 收斂後的設計結論

目前最合理的 solver 方向是：
- 表示法：`B*-tree`
- 搜尋主體：`multi-stage SA`
- move set：重新設計，不必照現有 `BTREE_SA`
- evaluator：`normalized area + wire + legality + congestion proxy`
- 壅塞修補：`hotspot-guided repair + virtual inflation + spacing adjustment`
- 特殊資源：`channel / FT / top-wire` 提早納入成本與修補
