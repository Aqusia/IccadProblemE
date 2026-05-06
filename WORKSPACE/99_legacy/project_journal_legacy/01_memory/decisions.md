# Decisions

## D-001 使用 `project_journal/` 作為唯一紀錄入口

內容：
- 所有專案紀錄集中在 `project_journal/`。

原因：
- 降低後續 AI 搜尋成本。

影響：
- 後續任何規劃、知識、實作、git 資訊都應優先更新到這裡。

## D-002 `01_memory/` 採分層結構

內容：
- 在 `01_memory/` 下區分：
  - 題目整理
  - survey 總覽
  - reusable techniques
  - `papers/` 逐篇筆記

原因：
- 題目規格與方法參考是兩種不同資訊流。

影響：
- 查題目時走 `problem_intro_E2026.md`
- 查方法時走 `paper_survey_overview.md` 與 `papers/`

## D-003 先以 heuristic / SA 類方法作為首選 baseline

內容：
- 第一版不考慮 RL。

原因：
- 題目有明確 legality 與 runtime 限制，heuristic / SA 更容易做出穩定可 debug 的 solver。

影響：
- 後續實作會優先考慮：
  - deterministic initialization
  - local search
  - constraints-aware SA

