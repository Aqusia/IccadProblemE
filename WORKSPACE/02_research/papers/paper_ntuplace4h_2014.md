# Paper: NTUplace4h (TCAD 2014)

原文：
- [NTUplace4h_A_Novel_Routability-Driven_Placement_Algorithm_for_Hierarchical_Mixed-Size_Circuit_Designs.pdf](/mnt/d/Bili/PD/FINAL/REFERENCE/NTUplace4h_A_Novel_Routability-Driven_Placement_Algorithm_for_Hierarchical_Mixed-Size_Circuit_Designs.pdf)

## 這篇在解什麼

作者在 mixed-size placement 中同時處理：
- large macro 與 cell 共存
- design hierarchy
- routability-driven placement

它特別強調四類 routability 問題：
- narrow channel
- pin density
- routing overflow
- net congestion

## 對你最重要的點

### 1. narrow channel handling
- 不是所有壅塞都長一樣
- 狹窄通道本身就是一種特殊失敗模式

這點和 E2026 很接近，
因為你本來就要處理 channel / feedthrough 類壓力。

### 2. pin density 不是小事
- 就算幾何上沒有 overlap
- pin 密度高的區域仍可能難 route

### 3. legalization / detailed placement 也要 routability-aware
- 不能只在 global stage 顧壅塞

## 可直接轉成你的 solver

- 額外做 `narrow-channel penalty`
- 對高 pin-count block 周圍提高 routing pressure
- 在 SA 後段做 `legality + congestion` 共同 repair

## 我對這篇的結論

這篇很適合幫你把 congestion 拆細：
- 不只 overflow
- 還有 channel width、pin access、macro porosity 類型問題
