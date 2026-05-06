# Paper: Routability-Driven Placement and White Space Allocation (TCAD 2007)

原文：
- [Routability-driven_placement_and_white_space_allocation.pdf](/mnt/d/Bili/PD/FINAL/Reference/Routability-driven_placement_and_white_space_allocation.pdf)

## 這篇在解什麼

重點不只是 congestion-driven placement，
而是：
- placement refinement
- white space allocation
要一起看。

## 核心方法

兩階段 flow：
1. refinement stage：
   - 用 congestion-weighted wirelength 做 placement 改善
2. global placement 後：
   - 依 congestion map 分配 white space
   - 透過 cut-line shifting 調整區域面積與 spacing

## 最重要的設計觀念

white space 不應平均分。
要依 congestion map 分配。

作者甚至明講：
- 兩個 technique 分開都有效
- 結合起來最好

## 對你現在的意義

這篇是你問「壅擠區怎麼處理、要不要放大 block」的最好答案之一。

但要精確理解：

### 不是盲目把 block 放大
更準確地說，是：
- 對壅塞區增加有效空間
- 方式可以是：
  - block inflation
  - whitespace reallocation
  - channel widening
  - moving neighbors away

### 對 hard macros
不要真的改 module size。
應改成：
- virtual inflation
- routing halo
- spacing pressure

### 對 soft blocks
才可考慮真的改 shape / area

## 這篇對 E2026 的直接轉譯

將「white space allocation」換成：
- explicit channel re-sizing
- congestion-driven gap expansion

具體做法：
1. 算每條 channel demand / overflow
2. 挑 top-k bottleneck channels
3. 針對相鄰 block 做：
   - 推開
   - 重排
   - 若可行，改走 FT
4. 重新估 cost

## 我對這篇的結論

對你要的 `B*-tree + SA`，這篇最該轉成：
- `post-move repair`
- `congestion-driven inflation/spacing`
- `targeted bottleneck expansion`

而不是只把 congestion 當 penalty 數字塞進 cost。

