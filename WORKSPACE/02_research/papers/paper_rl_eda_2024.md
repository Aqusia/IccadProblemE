# Paper: RL-EDA (2024)

原文：
- [Reinforcement-Learning-For-Automated-Chip-Floorplanning-And-Routing-Optimization.pdf](/mnt/d/Bili/PD/FINAL/REFERENCE/Reinforcement-Learning-For-Automated-Chip-Floorplanning-And-Routing-Optimization.pdf)

## 這篇在解什麼

它把 floorplanning + routing 視為：
- sequential decision problem
- 以 PPO 做 policy learning
- reward 同時顧 wirelength / timing / power / congestion

## 對你真正有用的點

### 1. reward decomposition
- 多目標不是一個模糊總分
- 可以拆成多個具體回饋來源

### 2. feedback-driven refinement
- 先從初始解出發
- 再根據回饋持續修正

這和 SA 精神其實不衝突。

## 不建議直接照抄的部分

- 這篇不是你這次最穩的主線
- RL training 成本高
- 對課堂 project 風險偏大
- 論文本身也較偏框架宣稱，不像經典 PD 論文那樣可直接轉實作細節

## 可借用的最小部分

- cost term 分解
- stage reward / acceptance 的設計方式
- 後續若要做 adaptive move selection，可把 RL 當未來方向

## 我對這篇的結論

把它放在：
- `future work`
- `adaptive move policy`
比較合理。

不建議把它當本次主線方法。
