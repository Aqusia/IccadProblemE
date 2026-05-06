# Contest Spec E2026

來源：
- [ICCAD_2026_Early_Floorplanning_with_Global_Route_Spec.pdf](/mnt/d/Bili/PD/FINAL/Reference/ICCAD_2026_Early_Floorplanning_with_Global_Route_Spec.pdf)

高層目標：
- Early floorplanning
- block placement
- global routing path planning
- channel sizing
- feedthrough usage
- wirelength / area / cost / legality 共同最佳化

本題與 PA2 的關鍵差異：
- `BTREE_SA/` 現在是 hard-macro fixed-outline floorplanning baseline
- E2026 還需要：
  - soft / hard / edge block 類型
  - channel capacity modeling
  - feedthrough conversion efficiency
  - path representation
  - top-wire / top-net routing thinking

所以：
- `BTREE_SA` 可以當 floorplan/search 核心
- 但 route estimator、channel model、FT model 都要新增

先保留的直接規格重點：
- channel density limit：`25 nets / um`
- HPWL 越小越好，但不能只優化 HPWL
- fail 至少包括：
  - overlap
  - outline violation
  - routing open
  - format error

建議搭配閱讀：
- `problem_intro_E2026.md`
- `../02_research/paper_survey_overview.md`

