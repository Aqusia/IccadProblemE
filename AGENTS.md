# 工作規範（給 Codex / 助手自己看）

目的：
- 這個根目錄同時包含：
  - `Reference/`：原始題目與論文 PDF
  - `BTREE_SA/`：實際 code repo
  - `WORKSPACE/`：整理後的知識、設計、repo 狀態、實驗筆記
- 後續所有工作都要把 raw/source/curated 分開，不要再把論文、code、整理筆記混在一起。

語言：
- 以中文為主，保留必要英文技術名詞。

## 目錄責任

### `Reference/`
- 放 raw PDF、官方 handout、外部補充論文。
- 原則上不在這裡寫分析。
- 若檔名太差，可改成可讀檔名。
- 要維護 `Reference/README.md` 作為 paper 對照索引。

### `BTREE_SA/`
- 唯一的實作 repo。
- 若之後要記：
  - git 狀態
  - branch / commit 進度
  - code module 分析
  - 實作改造計畫
  都以這個 repo 為準。

### `WORKSPACE/`
- 唯一的整理後知識入口。
- 後續應優先讀這裡，而不是先掃整個專案。
- 子目錄固定如下：
  - `WORKSPACE/01_problem/`
  - `WORKSPACE/02_research/`
  - `WORKSPACE/03_repo/`
  - `WORKSPACE/04_design/`
  - `WORKSPACE/05_runs/`
  - `WORKSPACE/99_legacy/`

## WORKSPACE 結構規則

### `01_problem/`
- 放題目規格、input/output、cost、fail condition。
- 優先從官方題目 PDF 整理。

### `02_research/`
- 放論文整理、方法比較、外部 research。
- `papers/` 放逐篇詳細筆記。
- 若要做演算法決策，先看這層。

### `03_repo/`
- 放 `BTREE_SA/` repo 的結構、git 狀態、目前實作內容。
- 這層是「研究」和「實作」之間的橋接層。

### `04_design/`
- 放目前採用或候選的 solver 設計。
- 這裡要回答：
  - 要用什麼表示法
  - 要用什麼 search
  - cost 怎麼拆
  - congestion 怎麼估
  - penalty 怎麼下
  - 如何把研究結論轉成 code

### `05_runs/`
- 放實驗、調參、結果比較。
- 若之後開始跑 sweep、benchmark、ablation，都記在這裡。

### `99_legacy/`
- 舊版結構或舊筆記收這裡，不直接刪。

## 命名規範

- 文檔名稱用可讀英文 snake/camel 混合皆可，但要明確。
- 逐篇 paper 筆記檔名優先：
  - `paper_<topic>_<venue_or_year>.md`
- 設計檔優先：
  - `btree_sa_hybrid_solver.md`
  - `penalty_and_congestion_model.md`
  - `repo_migration_plan.md`

## 工作順序

1. 先看 `WORKSPACE/README.md`
2. 先讀 `WORKSPACE/01_problem/` 與 `WORKSPACE/02_research/` 的導讀層檔案
3. 再看 `WORKSPACE/04_design/`
4. 看 `WORKSPACE/03_repo/`
5. 再進 `BTREE_SA/` 看實際 code

補充：
- `WORKSPACE/99_legacy/` 裡有一版功能較完整的舊分類。
- 若新結構缺少「題目解釋 / paper survey / reusable techniques / 多路線設計」這種導讀層，應優先把 legacy 的內容整合回來，而不是只保留偏 implementation / design 的精簡檔。

## 研究規則

- 若使用者要求「完整讀論文後再回答」：
  - 至少建立一份該論文的詳細筆記到 `WORKSPACE/02_research/papers/`
  - 筆記至少包含：
    - 問題設定
    - 表示法 / 演算法
    - cost / constraints
    - 關鍵操作
    - 實驗結論
    - 對本專案的直接啟示

- 若論文和 repo 直接相關：
  - 另外在 `WORKSPACE/04_design/` 建一份「如何改造現有 repo」的設計檔

## 實作規則

- 若修改 `BTREE_SA/` code：
  - 同步更新 `WORKSPACE/03_repo/` 與 `WORKSPACE/04_design/`
- 若新增演算法假設、懲罰項、stage schedule：
  - 優先記在 `WORKSPACE/04_design/`
- 若發現 paper 與 code 不一致：
  - 要明確寫出「paper idea」和「目前 code 現況」的差異

## Git 規則

- 根目錄 `/mnt/d/Bili/PD/FINAL` 不是 git repo。
- `BTREE_SA/` 是 git repo。
- 所有 git 相關狀態、branch、commit、working tree 差異，都只記 `BTREE_SA/`。

## 回報規則

- 回報使用者前，優先更新：
  - `WORKSPACE/02_research/` 若是研究性工作
  - `WORKSPACE/03_repo/` 若讀了 repo
  - `WORKSPACE/04_design/` 若形成設計結論
- 不要只在對話裡講結論，卻不把結論落到 `WORKSPACE/`
