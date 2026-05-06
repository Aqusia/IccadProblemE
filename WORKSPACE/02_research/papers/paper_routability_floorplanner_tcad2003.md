# Paper: Routability Driven Floorplanner with Buffer Block Planning (TCAD 2003)

原文：
- [Routability_Driven_Floorplanner_with_Buffer_Block_Planning.pdf](/mnt/d/Bili/PD/FINAL/Reference/Routability_Driven_Floorplanner_with_Buffer_Block_Planning.pdf)

## 這篇在解什麼

傳統 floorplanner 只看 area 已經不夠。
這篇主張：
- interconnect delay
- congestion
- routability
- buffer feasibility
都要提早進 floorplanning。

## 核心方法

### Two-phase SA
- Phase 1：area + wirelength 粗優化
- Phase 2：area + wirelength + congestion + routability

### Probabilistic congestion estimation
- 不只看單一路徑
- 對 route 使用概率做估計
- 轉成各區域/通道的期望壅塞

### Resource-aware thinking
- buffer insertion location 不是免費
- route feasibility 和資源可用性耦合

## 對你現在的意義

這篇最值得借的不是 buffer 本身，而是：
- routability 資源不能最後才看
- 需要兩階段 SA
- 需要一個比 HPWL 更接近 routing reality 的 evaluator

若你把 `buffer` 替換成：
- channel capacity
- feedthrough capacity
- top-wire resource

這篇幾乎可直接轉譯成 E2026 思路。

## 可直接套用到 B*-tree + SA

### Stage 設計
- Stage 1：area / HPWL / legality
- Stage 2：加入 congestion / route proxy
- Stage 3：針對 top overflow channels repair

### 成本項
- `base area + wire`
- `channel overflow`
- `FT overflow`
- `top-wire usage`

### evaluator 設計
- fast evaluator：bbox / RUDY 類 demand
- accurate evaluator：有限候選 path assignment

## 我對這篇的結論

這篇提供了你這條路線最核心的框架：
- B*-tree 負責 topology
- SA 負責 search
- routability evaluator 負責把壅塞壓力引回 search

