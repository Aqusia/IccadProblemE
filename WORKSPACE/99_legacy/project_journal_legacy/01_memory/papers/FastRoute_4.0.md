# FastRoute 4.0

原文：
- [FastRoute_4.0_Global_router_with_efficient_via_minimization.pdf](/mnt/d/Bili/PD/FINAL/FastRoute_4.0_Global_router_with_efficient_via_minimization.pdf)

題名：
- `FastRoute 4.0: Global Router with Efficient Via Minimization`

重點：
- 不把 via minimization 只當 maze routing cost 的附屬項。
- 在 Steiner tree generation、3-bend routing、layer assignment 全流程一起處理。
- 強調 secondary objective 必須在 routing flow 全程顯式建模。

對本題可用：
- 本題 secondary objective 是 FT / channel cost，而非 via。
- 觀念相同：不能只在最後才補 penalty。

可落地成：
- route candidate generation 時就記錄：
  - 經過哪些 channel
  - 是否使用 FT
  - 預估擴張成本

