# Paper: Are Floorplan Representations Important (ISPD 2005)

原文：
- [Are_Floorplan_Representations_Important_ISPD2005.pdf](/mnt/d/Bili/PD/FINAL/Reference/external/Are_Floorplan_Representations_Important_ISPD2005.pdf)

## 這篇在解什麼

這篇不是在發明新表示法，而是在質疑：
- 表示法真的那麼重要嗎？
- B*-tree、sequence pair 的差異，實際上會不會大到改變最終設計品質？

## 核心結論

作者的實驗結論偏保守：
- 在實際 floorplacement / mixed-size placement 內，representation 對最終品質影響有限
- 最終品質常常更受：
  - objective 設計
  - search flow
  - packing / legalization / downstream flow
影響

論文中甚至直接指出：
- sequence pair 對 B*-tree 的 wirelength 優勢不到 1%
- 差距小到很難說誰絕對更好

## 對你現在的意義

### 1. 不要陷入 representation 宗教戰
你現在已經有一個 `BTREE_SA` repo。
在這個前提下，繼續用 B*-tree 很合理。

更該花時間的地方是：
- search policy
- constraints-aware move
- congestion estimator
- penalty model
- legalization / repair

### 2. B*-tree 仍然是好選項
論文沒有否定 B*-tree。
它只是說：
- 不要期待「光換表示法」就帶來巨大提升

### 3. 若目標是 E2026
真正決定成敗的，不是 B*-tree vs SP 本身，而是：
- 是否能把 routing / channel / FT 壓力放進 search
- 是否能在不合法時快速修復
- 是否能對壅塞區做有效 re-spacing

## 我對這篇的結論

這篇的價值不是提供新招，而是幫你做決策：
- 用 B*-tree 沒問題
- 但後續時間不要再花在「要不要改 SP」上
- 應該直接投資在 `SA + constraints + congestion + repair`

