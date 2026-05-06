# NCTU-GR 2.0

原文：
- [NCTU-GR_2.0_Multithreaded_Collision-Aware_Global_Routing_With_Bounded-Length_Maze_Routing.pdf](/mnt/d/Bili/PD/FINAL/NCTU-GR_2.0_Multithreaded_Collision-Aware_Global_Routing_With_Bounded-Length_Maze_Routing.pdf)

題名：
- `NCTU-GR 2.0: Multithreaded Collision-Aware Global Routing with Bounded-Length Maze Routing`

重點：
- 提出 bounded-length maze routing，加速傳統 maze routing。
- route search 受長度上界限制，減少不必要探索。
- 用 task-based concurrency，而非單純 partitioning。
- 另外用 RSMT-aware routing guidance 改善 wirelength。

對本題可用：
- detour 路徑搜尋要有限界。
- 若之後要加速，可先平行化 per-net candidate evaluation。

可落地成：
- 對每條 connection 限制最多額外 detour 幾何長度或幾個 channel hop。

