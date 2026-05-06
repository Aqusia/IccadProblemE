# A SimPLR Method

原文：
- [A_SimPLR_method_for_routability-driven_placement.pdf](/mnt/d/Bili/PD/FINAL/A_SimPLR_method_for_routability-driven_placement.pdf)

題名：
- `A SimPLR Method for Routability-driven Placement`

重點：
- 把 global router 直接整合進 placement loop。
- 強調 lookahead routing，而不是用粗糙 congestion proxy。
- 會在壅塞區把 cell 撐開，在冷區把 cell 收回，兼顧 routability 與 wirelength。

對本題可用：
- 每次 floorplan move 後，馬上更新快速 routing estimate。
- 不要只依 HPWL 排序候選解。

可落地成：
- 建一個輕量 route evaluator：
  - 輸入 block rectangles + connection matrix
  - 輸出 channel demand / overflow / FT demand

