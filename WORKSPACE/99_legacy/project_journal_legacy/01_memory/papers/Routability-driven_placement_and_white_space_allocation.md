# White Space Allocation

原文：
- [Routability-driven_placement_and_white_space_allocation.pdf](/mnt/d/Bili/PD/FINAL/Routability-driven_placement_and_white_space_allocation.pdf)

題名：
- `Routability-Driven Placement and White Space Allocation`

重點：
- 兩階段 flow：
  - placement refinement with congestion-weighted wirelength
  - white space allocation based on congestion map
- white space 不是平均分，而是根據 congestion 分配。
- 可顯著改善 routability。

對本題可用：
- channel 本質上就是要顯式分配的 white space。
- 可用 congestion map 反推哪些 block 間 gap 要拉大。

可落地成：
- 每輪根據 channel overflow 排名
- 針對 top-k bottlenecks 做 block shifting / spacing

