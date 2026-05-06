# TSV-Aware 3D Floorplanning

原文：
- [Routability-Driven_TSV-Aware_Floorplanning_Methodology_for_Fixed-Outline_3-D_ICs.pdf](/mnt/d/Bili/PD/FINAL/Routability-Driven_TSV-Aware_Floorplanning_Methodology_for_Fixed-Outline_3-D_ICs.pdf)

題名：
- `Routability-Driven TSV-Aware Floorplanning Methodology for Fixed-Outline 3-D ICs`

重點：
- 在 fixed-outline 條件下同時處理 wirelength 與 routability。
- 與多數完全依賴 SA 的方法不同，較偏 deterministic methodology。
- 證明 deterministic 架構在複雜限制下可更有效率。

對本題可用：
- 本題也有 fixed-outline 味道。
- 可以先 deterministic 初始化，再用 local search 精修。

可落地成：
- 先做一個 greedy / constructive initial floorplan
- 再進入 cost-driven refinement

