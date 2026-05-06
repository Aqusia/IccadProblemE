# Paper: NCTU-GR 2.0 (TCAD 2013)

原文：
- [NCTU-GR_2.0_Multithreaded_Collision-Aware_Global_Routing_With_Bounded-Length_Maze_Routing.pdf](/mnt/d/Bili/PD/FINAL/REFERENCE/NCTU-GR_2.0_Multithreaded_Collision-Aware_Global_Routing_With_Bounded-Length_Maze_Routing.pdf)

## 這篇在解什麼

它主要在解：
- maze routing 很慢
- 傳統 router 雖能壓 overflow，但 wirelength 控制差
- 平行 routing 容易互相 collision

## 核心方法

### 1. bounded-length maze routing
- 不是毫無限制地找最小成本路
- 先限制可接受路徑長度範圍
- 大幅減少搜尋空間

### 2. RSMT-aware guidance
- 用樹結構先導引 routing
- 降低純 maze 搜尋的亂度

### 3. collision-aware parallel routing
- 平行處理時顧及多執行緒搶同一資源

## 對你現在的價值

你不一定需要 router 本體，
但這篇給兩個很實際的想法：

### 1. evaluator 要有 bounded approximation
- 不必每次追求最精確
- 先用受限搜尋得到穩定 proxy

### 2. move/repair 可以先用結構性導引
- 例如先鎖定最壅塞 net / block / channel
- 再做局部修補

## 可直接借用

- 對 route assignment 做 bounded detour estimate
- 只在 top-k 壅塞區做較精細的局部評估
- 其他區域維持便宜的粗估

## 我對這篇的結論

這篇不是你主線的核心參考，
但很適合用來約束 evaluator runtime：
- 精度可以局部加強
- 全域不必每步都跑重型求解
