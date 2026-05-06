# Penalty And Congestion Model

## 目標

把 cost 從「只有 area + HPWL + outline overflow」擴成：
- 可支撐 routability-aware search
- 但不讓 inner loop 太慢

## Level 1: base cost

沿用現有 repo 風格：

`base = alpha * normalized_area + (1 - alpha) * normalized_wire`

## Level 2: legality penalty

### outline overflow
沿用現有乘法型 penalty 很合理：

`overflowPenalty = lambda * ((1 + dx)(1 + dy) - 1)`

優點：
- 單維超出時近似線性
- 雙維超出時會額外懲罰交互項

### feasible lock
目前 repo 已有，應保留：
- 一旦進入 refinement 並合法，不再接受非法解

## Level 3: congestion penalty

### 第一版建議三項

#### 1. RUDY-like density
- 對每個 net 用 bbox 投影需求
- 形成 coarse heat map

#### 2. Bottleneck gap penalty
- 對相鄰大塊之間的狹窄 gap 給高罰
- 特別是高 net-degree blocks 之間

#### 3. Boundary pressure penalty
- 若高連線 block 被壓在角落或窄邊，給額外罰

綜合成：

`congestionPenalty = c1 * rudy + c2 * bottleneck + c3 * boundaryPressure`

## Level 4: inflation / spacing penalty

### Hard macro 模式
- `virtualHalo_i >= 0`
- effective rectangle = actual rectangle + halo margin

不要把這個寫回最終輸出尺寸。
它只是 search 的壓力模型。

#### halo 更新規則建議
- 若 block 周圍熱度高，增加 halo
- 若熱度下降，慢慢回縮 halo

#### spacing penalty
- 若兩塊 effective rectangles 過近
- 但 actual rectangles 尚未 overlap
- 給 soft penalty，促使 move 拉開

## 動態權重建議

### Stage 1
- legality 重於 congestion
- congestion 只做弱引導

### Stage 2
- legality 與 congestion 接近同等

### Stage 3
- legality 鎖住後
- 主要優化：
  - wirelength
  - congestion
  - spacing

## Top-k hotspot repair

不要只靠全域 cost。

每輪可額外抓：
- top-k hottest blocks
- top-k narrowest gaps
- top-k worst bbox density bins

用來：
- 偏置 move selection
- 插入 congestion-fixing move

