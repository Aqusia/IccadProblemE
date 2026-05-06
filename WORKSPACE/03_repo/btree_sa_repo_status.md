# BTREE_SA Repo Status

repo：
- [BTREE_SA](/mnt/d/Bili/PD/FINAL/BTREE_SA)

定位：
- 這不是空白 repo。
- 已經是一個 `B*-tree + multi-stage SA + fixed-outline overflow penalty` 的 PA2 floorplanner。

目前已經有：
- B*-tree representation
- contour packing
- fixed-outline overflow handling
- normalized search cost
- multi-start
- multi-stage SA
- feasible lock
- greedy legalize
- 多種 tree perturb moves

目前還沒有：
- 真正的 routing / congestion estimator
- white-space / channel model
- explicit inflation / halo model
- constraints-aware anchored blocks
- edge / soft / FT / channel 的 contest-specific modeling

所以最合理的策略不是重寫，而是：
- 在這個 repo 上逐步疊新模型

