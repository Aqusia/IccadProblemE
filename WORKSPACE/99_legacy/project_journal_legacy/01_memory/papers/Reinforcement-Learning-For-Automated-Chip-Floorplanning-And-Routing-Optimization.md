# RL-EDA

原文：
- [Reinforcement-Learning-For-Automated-Chip-Floorplanning-And-Routing-Optimization.pdf](/mnt/d/Bili/PD/FINAL/Reinforcement-Learning-For-Automated-Chip-Floorplanning-And-Routing-Optimization.pdf)

題名：
- `Reinforcement Learning For Automated Chip Floorplanning And Routing Optimization`

重點：
- 用 PPO 學 placement / routing policy。
- reward 同時考慮 wirelength、timing、power、congestion。
- 強調 multi-objective reward 設計。

對本題可用：
- 多目標 reward 拆解思路可借。
- 但不建議直接採 RL 當第一版 solver。

可落地成：
- 借 reward decomposition 觀念，轉成明確 cost terms。

