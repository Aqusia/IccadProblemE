# E_2026 題目介紹

來源：
- [E_20260414.pdf](/mnt/d/Bili/PD/FINAL/E_20260414.pdf)

題目名稱：
- `2026 ICCAD Contest - Early Floorplanning with Global Route`

## 題目定位

這題不是一般單純 floorplanning。
它要求在 very early stage，就同時做：
- block placement
- channel 規劃
- global routing path 規劃
- feedthrough 使用決策

核心目標是提早估計：
- wirelength
- chip area
- chip cost
- routing feasibility

## 問題本質

輸入只給 early-stage 規格，不是完整 RTL / placement database。
可用資訊以 block 層級為主，包括：
- block 類型
- estimated area
- shape / aspect ratio constraint
- interface / connection matrix
- outline 限制
- edge block 合法位置
- feedthrough conversion 效率

所以這題的重點不是精細 cell placement，而是：
- 用粗粒度 block model 建出合理 floorplan
- 讓 routing 可行
- 在 cost 上取得平衡

## 目標函式方向

從題目說明可直接確認，至少要同時關注：
- `Wire Length`
- `Chip Area`
- `Chip Cost`

題目後段也強調 evaluation 會看：
- HPWL 越小越好
- 所有 on-chip interconnection 必須完成 global routing，不能 open
- runtime 要控制在限制內

可推論實作上至少需要一個加權 cost：
- `cost = alpha * wirelength + beta * area + gamma * routing_penalty + delta * cost_term`

其中 routing penalty 至少應涵蓋：
- channel overflow
- feedthrough overflow
- outline violation
- block overlap

## Block 類型

### Soft Block
- 面積固定，但寬高可調。
- 有 aspect ratio range。
- 可允許 feedthrough。
- 若承接 feedthrough，可能需要額外面積。

### Hard Block / Macro
- 寬高固定。
- 不允許 feedthrough。
- routing 必須繞經外部 channel。

### Edge Block
- 固定在 chip 邊界。
- 合法位置受輸入 constraint 限制，例如 `TL / TM / TR / BL / BM / BR / LT / LM / LB / RT / RM / RB`。
- 不允許 feedthrough。

## Routing / Path 模型

題目中的 route 允許經過：
- channel
- 可做 feedthrough 的 soft block
- detour path
- top net routing

關鍵名詞：
- `Connection flyline`：邏輯上的 block-to-block 連線需求
- `Top net route`：實際 global routing 路徑
- `Feedthrough`：net 從 block 一側進、另一側出，借道 block 內部
- `Channel`：block 之間保留給 routing 的空間

## Channel 規則

題目明確給出 channel density limit：
- `25 nets / um`

這個限制可直接轉成最小 channel width：
- `required_channel_width = routed_nets / 25`

因此 channel 大小不是任意，而是與：
- 穿越該 channel 的 net 數量
- 是否把部分 net 轉成 feedthrough
直接耦合。

## Feedthrough 規則

題目有 feedthrough conversion efficiency table。
依照 net 數區間，FT 轉換效率不同，例如文件範例中出現：
- `20%`
- `40%`
- `80%`
- `100%`

其含義可作為：
- 不同 net 規模下，允許多少 routing 壓力被轉進 soft block 內部
- 或需要為 feedthrough 額外擴張多少面積

這代表 feedthrough 不是免費資源，必須在：
- channel 面積增加
- soft block 擴張
之間權衡。

## 幾何 / 合法性要求

至少要滿足：
- block 不重疊
- 所有 block 在 outline 內
- edge block 在合法邊界位置
- hard block 尺寸固定
- soft block 尺寸需符合 area 與 aspect ratio
- channel 需對應實際可用 routing 空間

## Output 形式

題目輸出是 `*.cfg`，至少包含：
- floorplan outline
- block shape and location
- channel representation
- routing pattern representation

route path 不是只要起終點，還要描述經過哪些 block/channel 邊。

## Evaluation 重點

從文件第 14 頁可整理出：
- HPWL 越小越好
- 所有連線必須完成 global routing
- 可經 channel 或允許 FT 的 block
- runtime 需在 testcase 限制內

Penalty condition：
- channel overflow
- feedthrough overflow

Fail condition：
- format failed
- block overlap
- routing open
- outline constraint violation

## 對實作的直接啟示

### 1. 先做粗粒度而可計算的 routing model
題目本質上不是 detailed routing。
需要的是早期估計，所以模型必須：
- 快
- 能反映 congestion
- 能把 feedthrough 納入

### 2. channel / FT / block size 是聯動的
不是先放 block 再做 route。
因為：
- block 擺法會影響 channel
- channel 不夠會逼出 FT
- FT 又會回頭撐大 soft block

### 3. 成本函式不能只看 HPWL
若只壓 HPWL，很容易做出：
- channel 不夠寬
- FT 超量
- outline 爆掉

### 4. 題目很適合用「分階段 heuristic」
先求合法且粗可行解，再逐步改善：
- block placement
- route assignment
- channel sizing
- FT assignment
- cost refinement

## 建議最小可行解方向

第一版 solver 可先做：
1. 依 edge constraints 先固定 edge blocks
2. 放 hard blocks
3. 放 soft blocks，初始用 area 開方當邊長
4. 依 connection matrix 建 block-level net demand
5. 建立 channel graph
6. 用簡化 Manhattan / detour path 做 routing assignment
7. 估每條 channel demand，反推 required width
8. 若 demand 過大，嘗試把部分 demand 改走 soft block FT
9. 重新調整 soft block 大小與位置
10. 以 cost function 做 iterative improvement

## 目前待補

- input format 細節與 parser 欄位整理
- output `.cfg` 欄位的完整範例
- cost function 是否有官方權重或 evaluator 行為
- testcase 與 reference code 的使用方式

