# Discussion Log

## 2026-05-06

### 專案紀錄結構重設
- 原本想用 5 份平面 md。
- 後來改成 `project_journal/` + 5 個固定子資料夾。
- 原因：之後 AI 搜尋時只要先讀 index 與對應區塊，不必每次全掃專案。

### Folder 1 的用途定義
- `01_memory/` 主要放：
  - 題目介紹
  - paper survey
  - 可重用知識點
  - 決策與風險

### 題目來源確認
- 題目介紹以 `E_20260414.pdf` 為主。
- 其他 paper 主要作為方法與知識來源，不當作規格本身。

### 外部 research 補強方向
- 補查了 STAIRoute、HGR、RUDY、OpenROAD routability-driven placement、B*-tree 等資料。
- 結論是最值得直接轉成 code 的不是更多名詞，而是：
  - `constraints-aware move`
  - `fast_eval + accurate_eval`
  - `channel/FT` 分離建模
  - `bounded detour`
  - `inflation / widening / spacing`
