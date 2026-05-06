# Risks And Open Questions

## 目前風險

### R-001 題目 PDF 中文抽取有編碼失真
- `E_20260414.pdf` 可抽文字，但中文有部分亂碼。
- 目前已靠英文標題、可辨識術語、數字規則整理出主要結構。

影響：
- 需要後續再補 input/output 細節時，可能要改用人工閱讀或影像輔助確認。

### R-002 目前資料夾不是 git repo
- 無法直接產生 commit / branch 對應紀錄。

影響：
- `04_git_progress/` 與 `05_changes_since_git/` 只能先維持狀態說明模板。

## 待確認問題

- 題目 evaluator 的正式 cost 權重是否公開
- testcase input 檔的完整格式
- official reference code 的行為範圍
- top wire area 的精確計算方式
- feedthrough conversion efficiency 的精確數學套用方式

