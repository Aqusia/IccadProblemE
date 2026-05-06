# Paper: Improved Global Routing through Congestion Estimation (DAC 2003)

原文：
- [Improved_Global_Routing_through_Congestion_Estimation.pdf](/mnt/d/Bili/PD/FINAL/REFERENCE/Improved_Global_Routing_through_Congestion_Estimation.pdf)

## 這篇在解什麼

作者想改善 global routing 中：
- overcongested edges 太多
- rip-up and reroute 容易卡在壅塞區

核心想法是：
- 先估壅塞
- 再把壅塞資訊放大後回灌到 routing cost
- 讓後續路徑主動避開熱點

## 核心方法

### 1. amplified congestion estimate
- 不是只把目前 demand 當成本
- 而是對高壅塞區做放大
- 讓 router 對 hotspot 更敏感

### 2. static + dynamic feedback
- static：機率式 congestion estimation
- dynamic：前幾輪 rip-up/reroute 觀察到的壓力
- 兩者合併成 composite edge weight

## 對你現在的價值

這篇很適合轉成 `SA` 的 penalty 設計：

### 1. penalty 不該線性、平均
- 對接近爆掉的 channel / boundary / FT 要急遽加重
- 才能讓 search 願意離開壞區域

### 2. 要有歷史記憶
- 一直重複壅塞的 hotspot，權重應逐步提高
- 可做成短期 history-based hotspot bias

## 可直接借用

- `overflow^2` 或 piecewise overflow penalty
- hotspot history table
- repeated-violation guided move selection

## 我對這篇的結論

這篇最值得借的是：
- amplified penalty
- static + dynamic congestion feedback

它很適合當你 `congestion-repair stage` 的成本設計參考。
