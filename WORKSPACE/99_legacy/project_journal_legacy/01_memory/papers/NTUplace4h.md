# NTUplace4h

原文：
- [NTUplace4h_A_Novel_Routability-Driven_Placement_Algorithm_for_Hierarchical_Mixed-Size_Circuit_Designs.pdf](/mnt/d/Bili/PD/FINAL/NTUplace4h_A_Novel_Routability-Driven_Placement_Algorithm_for_Hierarchical_Mixed-Size_Circuit_Designs.pdf)

題名：
- `NTUplace4h: A Novel Routability-Driven Placement Algorithm for Hierarchical Mixed-Size Circuit Designs`

重點：
- routability 最佳化被拆成四塊：
  - narrow channel handling
  - pin density
  - routing overflow optimization
  - net congestion optimization
- 還包含 routability-driven legalization 與 detailed placement。

對本題可用：
- cost function 要拆細，不可只一個 congestion penalty。
- narrow channel 應有獨立權重。
- edge / interface 集中區可能要用類 pin density 的概念看待。

可落地成：
- 計算每個 block side 的 interface load
- 檢查 block 間形成的 bottleneck gap

