# Paper Notes: RUDY / OpenROAD

來源：
- RUDY: [DATE 2007 PDF](https://websrv.cecs.uci.edu/~papers/date07/PAPERS/2007/DATE07/PDFFILES/08.7_1.PDF)
- OpenROAD docs: [Global Placement docs](https://openroad.readthedocs.io/en/latest/main/src/gpl/README.html)

## RUDY 的核心

RUDY = `Rectangular Uniform wire DensitY`

概念：
- 對每個 net，用其 bounding box 估 wire density
- 把 demand 均勻投影到覆蓋區域
- 不做真正 routing，也能快速估壅塞熱區

優點：
- 極快
- 適合 inner loop

缺點：
- 對 detour、resource-aware path、feedthrough 建模較粗

## OpenROAD 的做法

OpenROAD 在 routability-driven placement 中：
- 每輪跑 RUDY
- 找出 congestion 高的 tiles
- 對那裡的 cell area inflation
- 若 RC 連續幾輪不下降就停

## 對你現在的直接意義

### 1. 很適合當 fast evaluator
`BTREE_SA` 不可能每個 move 都做重 routing。
所以需要：
- fast evaluator：RUDY 類
- accurate evaluator：candidate path / channel assignment 類

### 2. inflation 應做成 virtual
對 hard macro repo：
- 不改 block 真尺寸
- 改成 congestion score / attach score / local spacing pressure

### 3. 可加入 stop rule
若某 round 的 overflow / RC 連續不降：
- 進下一 stage
- 或做 diversification / repair

