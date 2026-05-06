# Paper: SimPLR (Routability-Driven Placement)

原文：
- [A_SimPLR_method_for_routability-driven_placement.pdf](/mnt/d/Bili/PD/FINAL/REFERENCE/A_SimPLR_method_for_routability-driven_placement.pdf)

## 這篇在解什麼

作者要解的是：
- 傳統 placement 只壓 HPWL，最後常導致 routing 爆掉
- 單靠粗糙 congestion model 不夠，尤其在多層金屬、障礙物、via 規則存在時
- post-placement 小修補常救不回全域結構問題

所以它把：
- fast global routing
- global placement
做成更緊的 lookahead feedback loop。

## 核心方法

### 1. lookahead routing
- 不是只用 bbox density 猜壅塞
- 直接在 placement 過程中跑快速、layer-aware 的 global routing 估計
- 提前知道 trouble spots

### 2. simultaneous place-and-route
- 壅塞區把 cell 往外推
- 不壅塞區允許 cell 靠近，避免 wirelength 無限制膨脹
- 不是單方向地「一直擴散」

### 3. dynamic whitespace control
- 空白不平均分配
- 而是根據 congestion heat map 定向釋放空間

## 對你現在的價值

這篇不是直接給 `B*-tree` move，
但提供了很重要的 evaluator / repair 思路：

### 1. congestion 不能只靠靜態 penalty
- 最好有 routing proxy 或 lookahead routing
- 至少要比純 HPWL 更接近真實 demand

### 2. 壅塞修復不是單純放大整體 outline
- 要對 hotspot 做局部疏散
- 冷區反而可以重新壓緊

### 3. SA 可以保留，但 move 選擇要受到 congestion 導引
- 挑 hotspot 附近 block 做 targeted perturbation
- 而不是完全均勻隨機

## 可直接轉成你的 solver

可借用成：
- `fast congestion evaluator`
- `hotspot-guided move selection`
- `local spreading + cool-region recompaction`

對 hard macro 題目不必照抄 cell placer，
但概念可以轉成：
- hotspot channel 周邊 block 加較大 spacing pressure
- 非 hotspot 區維持較高 compaction

## 我對這篇的結論

這篇最值得借的不是「整套 placer」，
而是：
- SA 內層要有 routability feedback
- hot region 要定向修，不要平均修
- congestion-aware 修補要和 wirelength tradeoff 一起考慮
