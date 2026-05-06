# Solver Routes Detailed

目的：
- 把目前可行的 solver 方案拆成多條路線
- 每條路線都要能回答：
  - 核心想法
  - 需要哪些資料結構
  - 演算法流程
  - 優缺點
  - 何時該選

## Route A: 最穩妥 Contest Baseline

定位：
- 第一版最推薦
- 重點是先做出穩定合法、可 debug、可逐步加強的解

核心想法：
- 先做合法 block placement
- 再做 block-level route estimation
- 再根據 channel / FT 壓力迭代修正

建議資料結構：
- `Block`
  - id
  - type: `EDGE | HARD | SOFT`
  - area
  - width / height
  - aspect ratio range
  - fixed edge constraint
  - current rectangle
- `Connection`
  - src block
  - dst block
  - net count
- `Channel`
  - id
  - rectangle
  - adjacent block pair
  - demand
  - capacity
  - overflow
- `PathCandidate`
  - ordered segments
  - total length
  - used channels
  - used feedthrough blocks
  - predicted penalties
- `Solution`
  - blocks
  - channels
  - assigned paths
  - total cost breakdown

流程：
1. 固定 edge blocks 到合法邊界區
2. 放 hard blocks
3. soft blocks 用 `sqrt(area)` 初始化尺寸
4. 建 block adjacency / geometry induced channel graph
5. 對每個 connection 生成少量候選路徑
   - 直走
   - 1 次 detour
   - 經 1 個 soft block FT
6. 根據所有 connection 累積 channel demand
7. 用 `required_width = demand / 25` 估 channel 最小寬度
8. 若 channel overflow，嘗試：
   - 換別的 path
   - 啟用 FT
   - 拉開 bottleneck block gap
9. 重算 cost
10. 用 local search 反覆修正

move set 建議：
- swap 兩個 non-edge blocks
- shift block x/y
- resize soft block within aspect ratio
- rotate hard block if題目允許
- move one block away from congested channel
- change connection path assignment
- toggle partial FT usage on candidate soft block

cost 建議：
- `w1 * HPWL`
- `w2 * estimated_area`
- `w3 * total_channel_overflow`
- `w4 * total_feedthrough_overflow`
- `w5 * overlap_penalty`
- `w6 * outline_penalty`
- `w7 * narrow_channel_penalty`
- `w8 * edge_violation_penalty`

優點：
- 實作門檻低
- 每個模組都好 debug
- 後續容易插入更強 estimator

缺點：
- 若候選路徑生成太弱，會錯過更好解
- 若 move 太 local，可能卡住

適合：
- 現在就該做

## Route B: Constraints-Aware SA

主要來源：
- PARSAC
- two-phase floorplanning papers

定位：
- 若你想做比較像正式 floorplanner 的架構，這是最合理的主線

核心想法：
- 用 simulated annealing 搜尋
- 但 move generator 本身就知道限制，不浪費在明顯非法解
- 先 area/legality phase，再 route/congestion phase

流程：
1. 建立合法初始解
2. Phase 1 cost：
   - area
   - HPWL
   - overlap / outline legality
3. Phase 2 cost 再加：
   - channel overflow
   - FT overflow
   - congestion hotspot
   - narrow channel penalty
4. 每輪 move 後跑快速 route evaluator
5. 接受準則使用 SA

必做的 constraints-aware move：
- edge block 只能在 allowed edge slots
- hard block 尺寸不可亂改
- soft block resize 後仍滿足 area 與 ratio
- 對超過 outline 的 move 直接拒絕或投影回合法區

很重要的工程細節：
- 不要每輪都 full reroute 所有 connection
- 只重算受影響 block 附近的 channel 與 path

推薦 phase 切法：
- `Phase 1`: legality + area + rough HPWL
- `Phase 2`: route-aware refinement
- `Phase 3`: targeted repair on top congested channels

優點：
- 對複雜約束很自然
- 容易疊更多 cost 項

缺點：
- 若 evaluator 太慢，runtime 會炸
- 需要小心調退火參數

適合：
- baseline 做完後，最值得升級的方向

## Route C: B*-Tree / Sequence Pair 正規表示法版

主要來源：
- B*-Tree paper
- sequence pair 系列
- `Are Floorplan Representations Important`

定位：
- 若你要把這題當研究型 floorplanner 做，不只是 contest code

核心想法：
- 用標準 floorplan representation 管理幾何關係
- 用 representation 的 move 來保證 packing 合理

### C1. B*-Tree 路線

適合原因：
- 對 hard/pre-placed/soft 模組的直接操作簡單
- 文獻顯示它對約束整合很自然

你可以怎麼用在本題：
- 每個 block 對應 tree node
- decode 後得到 packing
- edge blocks 不完全放進 tree，自成固定 anchor
- soft block 可在 decode 後再做局部 reshape

風險：
- channel 與 FT 是 packing 之後才看得到，整合難度較高

### C2. Sequence Pair 路線

適合原因：
- 非 slicing 表示力強
- 對複雜相對關係好表達

風險：
- move 與 decode 較重
- 若要高頻重算 routing evaluator，會比較吃 runtime

建議：
- 如果要很快落地，優先 B*-tree
- 如果想保留更大表示自由度，再考慮 SP

## Route D: Deterministic Constructive + Repair

主要來源：
- interconnect-driven floorplanning
- TSV-aware deterministic methodology
- industrial planning logic

定位：
- 想避免 SA 參數敏感
- 想快速拿一個穩定但可能不夠漂亮的解

流程：
1. 依 edge constraints 固定 edge blocks
2. 依 net demand / area 排序 hard blocks
3. 先放最大且連線最重的 blocks 在較中心或較有路由彈性的位置
4. soft blocks 補空隙
5. 建 channel
6. route all connections
7. 找 top-k overflow channels
8. 執行 repair：
   - 拉開 block
   - 插入 / 放大 channel
   - 改走 FT
9. 直到合法或達 iteration limit

repair heuristic 建議：
- 若單一 channel demand 很大：
  - 先檢查是否有可用 soft block 可作 FT
- 若某個 edge block 附近壅塞：
  - 嘗試把鄰近 soft block 往內縮
- 若兩個 macro 夾出狹窄瓶頸：
  - 優先做 gap expansion

優點：
- 可解釋性最好
- 實作和 debug 最簡單

缺點：
- 全域最優能力較差
- 容易受初始排序影響

適合：
- 當作最小可行版本，或 SA 的初始化器

## Route E: Fast Estimator-First 路線

主要來源：
- RUDY
- OpenROAD routability-driven placement
- white-space allocation

定位：
- 想把大部分力氣放在 fast evaluator，而不是複雜 search

核心想法：
- 用超快 congestion / routing demand estimator
- 在每輪改動後快速得到 heatmap
- 再做 inflation / whitespace reallocation

可直接借的觀念：
- `RUDY`: 每個 net 在 bounding box 上分配均勻 wire density
- `OpenROAD`: 擁塞區膨脹 cell area，降低 routing demand / 提高有效供給

你可以在本題改寫成：
- 對 block-pair connection 先用 connection bbox 估 demand
- 把 demand 投影到穿越的 channel / region
- 對高 demand 區：
  - 增加 channel width
  - 推開鄰近 soft blocks
  - 增加 soft block FT area

優點：
- 很快
- 適合放在 inner loop

缺點：
- 精度比顯式 path assignment 弱
- 若只靠 bbox density，detour / FT 影響會被低估

適合：
- 與 Route A/B 混用
- 當 early screening cost

## Route F: STAIRoute / HGR 啟發式路線

主要來源：
- STAIRoute
- HGR

定位：
- 若想做「比 bbox 更像真正早期 routing」的 evaluator

核心想法：
- routing region 不只在 block 間空白區
- 還能分層看 block boundary / block 上方 free region

對本題可借的最實際部分：
- 候選 route 不一定只走空白 channel，也可考慮更抽象的 region graph
- 若題目中的 top wire area 要認真做，這條路特別有價值

簡化落地版：
1. 建 lower-level channel graph
2. 建 upper-level over-block graph
3. route 時允許：
   - lower path
   - upper/top wire path
   - mixed path
4. 對 upper path 加上額外 cost，反映 top wire area / cost

優點：
- 更貼近題目中的 `top net routing`
- 容易把 top wire area 納入

缺點：
- 複雜度比 Route A 高
- 需要再定義多層容量模型

適合：
- baseline 穩後再做

## Route G: 不建議當第一版的路線

### G1. RL / GNN 直接做 end-to-end

原因：
- 建模、資料、reward、runtime、穩定性成本都太高
- contest first-pass 不需要

### G2. 一開始就全域 ILP / SMT

原因：
- 可當小規模 refinement
- 但不適合作為全問題主 solver

## 最推薦的實際採用方式

### 方案 1
- 主骨架：`Route A`
- inner evaluator：`Route E`
- repair：`Route D`

適合：
- 最快開始寫 code

### 方案 2
- 主骨架：`Route B`
- 初始化：`Route D`
- fast screen：`Route E`

適合：
- 想要較完整、較有競賽潛力的版本

### 方案 3
- floorplan representation：`Route C (B*-tree)`
- routing evaluator：`Route A + F`
- optimization：`Route B`

適合：
- 想做成研究型專題

## 我的建議

如果目標是先把題目做起來：
- 先採 `方案 1`

如果第一版跑通後要升級：
- 再走 `方案 2`

如果你打算把這題做成長期研究主題：
- 再考慮 `方案 3`

