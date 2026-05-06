# Web Research Sources

目的：
- 記錄外部 research 來源
- 只保留對目前題目和 `B*-tree + SA` 主線有直接價值的點

## 1. PARSAC

來源：
- [arXiv abstract](https://arxiv.org/abs/2405.05495)
- [IntelLabs/parsac GitHub](https://github.com/IntelLabs/parsac)

可直接借：
- `constraints-aware simulated annealing`
- constraint-fixing moves
- anchored/pre-placed 觀念

## 2. Routability-Driven Floorplanner with Buffer Block Planning

來源：
- [TCAD paper PDF](https://www.cse.cuhk.edu.hk/~fyyoung/paper/tcad03_4b.pdf)

可直接借：
- two-phase SA
- congestion 期望估計
- resource-aware routing thinking

## 3. White Space Allocation

來源：
- 本地 PDF

可直接借：
- congestion-driven whitespace redistribution
- bottleneck expansion

## 4. RUDY

來源：
- [DATE 2007 PDF](https://websrv.cecs.uci.edu/~papers/date07/PAPERS/2007/DATE07/PDFFILES/08.7_1.PDF)

可直接借：
- bbox wire density 投影
- ultra-fast congestion estimate

## 5. OpenROAD

來源：
- [OpenROAD global placement docs](https://openroad.readthedocs.io/en/latest/main/src/gpl/README.html)

可直接借：
- 每輪做 congestion estimate
- inflation / RC-driven stop rule

## 6. B*-Trees

來源：
- [DAC 2000 PDF](https://websrv.cecs.uci.edu/~papers/compendium94-03/papers/2000/dac00/pdffiles/27_1.pdf)

可直接借：
- B*-tree 本體與 move 思路

## 7. Are Floorplan Representations Important

來源：
- [ISPD 2005 PDF](https://web.eecs.umich.edu/~imarkov/pubs/conf/ispd05-fp.pdf)

可直接借：
- 不要過度糾結 representation，本專案應把重點放在 cost / flow / repair

## 這輪 research 的結論

最值得立即轉成 code 的，不是更多 paper，而是以下 6 件事：
- 建 `fast_eval` 和 `accurate_eval` 雙層 evaluator
- 把 `constraints-aware move` 納入主 search
- 把 `outline legality`、`congestion`、`spacing` 分離建模
- 允許 `targeted repair`
- 對壅塞區做 `virtual inflation / widening / spacing`
- 保留升級到 `channel / FT / top-wire` 的介面

