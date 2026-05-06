# Paper: Routability-Driven Floorplanning of Analog and Mixed-Signal Circuits (Thesis, 2014)

原文：
- [Routability_Driven_Floorplanning_of_Analog_and_Mixed_Signal_Circuits_Thesis.pdf](/mnt/d/Bili/PD/FINAL/REFERENCE/Routability_Driven_Floorplanning_of_Analog_and_Mixed_Signal_Circuits_Thesis.pdf)

## 這篇在解什麼

這本 thesis 處理的是：
- 類比 / mixed-signal 佈局限制很多
- 不只 area / wirelength
- 還有 symmetry、alignment、boundary、preplace、abutment、range、maximum separation
- 同時還要顧 routability

## 論文中的核心做法

作者提出兩條路：
- Sequence Pair (SP) 做 mixed-constraint placement
- Polish Expression (PE) 處理 slicing 結構

而和你最有關的點是：
- 根據 net congestion probability 做 module expansion
- 透過擴張模組來保留 routing channel

## 對你最重要的點

### 1. congestion probability 可以驅動幾何調整
- 高壅塞模組周圍應保留更多空間
- 不是全部模組一視同仁

### 2. virtual expansion 這個思路是對的
- 對 hard macro 題目，不一定真的改實體尺寸
- 但可以改 effective spacing / halo / inflation

### 3. constraint-rich floorplanning 不應只靠單一 penalty
- 多類型約束應分開建模
- 才知道是 symmetry/boundary/routability 哪種在壞

## 可直接借用

- `module-specific virtual inflation`
- `probabilistic congestion around blocks`
- `separate penalties for different violation classes`

## 我對這篇的結論

這篇和你問的「壅塞區要不要做放大 block」最接近。

我的結論仍然是：
- 對 hard macro 先做 virtual inflation
- 不急著改真實 geometry
- 但這個方向本身是有文獻支撐的
