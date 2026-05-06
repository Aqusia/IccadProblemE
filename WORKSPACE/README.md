# WORKSPACE

這裡是主入口。

使用順序：
1. `01_problem/problem_intro_E2026.md`
2. `01_problem/contest_spec_e2026.md`
3. `02_research/paper_survey_overview.md`
4. `02_research/solver_routes_detailed.md`
5. `02_research/reusable_techniques.md`
6. `02_research/web_research_sources.md`
7. `04_design/btree_sa_hybrid_solver.md`
8. `03_repo/btree_sa_repo_status.md`

結構說明：
- `01_problem/`：題目規格
- `02_research/`：論文與方法研究
- `03_repo/`：`BTREE_SA` repo 現況
- `04_design/`：目前採用或候選的 solver 設計
- `05_runs/`：實驗與 sweep
- `99_legacy/`：舊版筆記封存

目前重點：
- 主線研究改成 `B*-tree + SA + constraints-aware penalties + congestion repair/inflation`
- 研究與設計都要對齊 `BTREE_SA/` 現有 repo，而不是另起爐灶
- 新版結構已重新吸收 `99_legacy` 裡較完整的導讀層，避免只有設計檔、沒有背景說明

導讀層：
- `01_problem/` 負責題目解釋與規格
- `02_research/` 負責 paper survey、可重用技術點、多路線設計與外部來源
