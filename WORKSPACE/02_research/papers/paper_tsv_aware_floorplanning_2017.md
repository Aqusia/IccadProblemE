# Paper: TSV-Aware Routability-Driven 3D Floorplanning (TCAD 2017)

原文：
- [Routability-Driven_TSV-Aware_Floorplanning_Methodology_for_Fixed-Outline_3-D_ICs.pdf](/mnt/d/Bili/PD/FINAL/REFERENCE/Routability-Driven_TSV-Aware_Floorplanning_Methodology_for_Fixed-Outline_3-D_ICs.pdf)

## 這篇在解什麼

它在 fixed-outline 3D floorplanning 裡同時考慮：
- modules
- TSV locations
- wirelength
- routability

作者特別強調：
- TSV 很大
- 位置不當會直接製造壅塞與繞路
- 不應只靠純 SA 硬搜

## 核心訊息

### 1. 關鍵資源要提早一起規劃
- 不能先放 module，再最後補 TSV
- 資源位置本身就是 floorplanning 的一部分

### 2. deterministic procedures 可和 heuristic 混搭
- 不一定全程都靠 SA
- 某些結構化子問題可用 deterministic repair 解

## 對你現在的價值

可類比成：
- TSV 對 3D 的角色
- 很像 E2026 中受限 routing resource / top-wire / FT / channel

所以最可借的是：
- 特殊 routing 資源要早納入
- SA 外圍要有 deterministic repair

## 可直接借用

- `resource-aware constructive initialization`
- `deterministic channel/FT repair`
- `special-resource reservation` 類成本項

## 我對這篇的結論

這篇強化了你的混合法方向：
- 主搜尋可用 SA
- 但對特殊資源衝突，應該搭 deterministic 修補
