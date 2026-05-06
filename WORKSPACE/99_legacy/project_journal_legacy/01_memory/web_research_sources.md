# Web Research Sources

目的：
- 記錄這輪外部 research 的來源
- 只保留對目前題目有直接價值的點

## 1. PARSAC

來源：
- [arXiv abstract](https://arxiv.org/abs/2405.05495)
- [IntelLabs/parsac GitHub](https://github.com/IntelLabs/parsac)

可直接借：
- `constraints-aware simulated annealing`
- 邊界 constraint、pre-placed block、aspect ratio 類約束要進 move generator
- GitHub README 還直接展示了一種 block tensor schema，可當 data model 參考

對本題價值：
- 題目有 edge block 邊界限制、hard/soft block 差異、outline 限制，和 PARSAC 非常對味

## 2. Routability-driven Floorplanner with Buffer Block Planning

來源：
- [TCAD paper PDF](https://www.cse.cuhk.edu.hk/~fyyoung/paper/tcad03_4b.pdf)

可直接借：
- two-phase SA
- congestion 期望估計
- intersection-to-intersection I/O pin estimation
- multipin net 先拆成 two-pin net

對本題價值：
- 題目中的 FT / channel 也屬於 routing resource feasibility 問題

## 3. STAIRoute

來源：
- [arXiv abstract](https://arxiv.org/abs/1810.10412)

可直接借：
- early global routing using pattern routing
- floorplan topology 層級就做 routability / wirelength / via estimation
- monotone staircase / region-based routing 觀念

對本題價值：
- 可延伸到 `top wire routing` 與抽象 routing region graph

## 4. HGR

來源：
- [arXiv abstract](https://arxiv.org/abs/1810.12789)

可直接借：
- generalized routing model
- lower layers 走 block boundary region
- higher layers 走 block over-the-top free regions

對本題價值：
- 非常適合思考題目裡的 `top wire area`
- 可做成 `channel graph + upper graph` 的雙層模型

## 5. Interconnect-Driven Floorplanning

來源：
- [UT Austin PDF](https://users.ece.utexas.edu/~dpan/publications/techcon00_idfp.pdf)

可直接借：
- 2-bend / Z-shape estimation
- GA-tree estimation
- runtime/accuracy tradeoff：早期先用快 estimator，後期再用較準 evaluator

對本題價值：
- 可以定義：
  - `fast_eval`: bbox / 2-bend
  - `accurate_eval`: explicit path assignment

## 6. Fast Congestion Evaluation / Twin Binary Trees

來源：
- [Iowa State PDF](https://home.engineering.iastate.edu/~cnchu/pubs/c17.pdf)

可直接借：
- 若 grid-based evaluator 太慢，可改用較抽象的 boundary wire density

對本題價值：
- 如果顯式 channel routing 太慢，可退成 region boundary density model

## 7. RUDY

來源：
- [DATE 2007 PDF](https://websrv.cecs.uci.edu/~papers/date07/PAPERS/2007/DATE07/PDFFILES/08.7_1.PDF)

可直接借：
- `Rectangular Uniform wire DensitY`
- 每個 net 以 enclosing rectangle 的 wire density 投影 demand
- 同時結合 direct 和 indirect routability optimization

可直接套用的式子：
- 若某 net 的估計 wirelength 是 `L_n`
- bbox 寬高為 `w_n`, `h_n`
- 則密度可視作 `d_n = (L_n * p) / (w_n * h_n)`
- 實作上可以省略比例常數 `p`，只保留相對量做 ranking

對本題價值：
- 可當 ultra-fast congestion screen
- 適合放在 local search / SA 內層

## 8. OpenROAD routability-driven placement

來源：
- [OpenROAD global placement docs](https://openroad.readthedocs.io/en/latest/main/src/gpl/README.html)

可直接借：
- routability-driven 模式每次迭代都執行 RUDY
- 擁塞 tile 會被 inflation，以降低 congestion
- 若 RC 不再下降，提早停止

對本題價值：
- 可把 block inflation 概念改造成：
  - channel widening
  - soft block FT area inflation
- 也可加 stop condition：
  - 若 overflow 連續幾輪不降就跳出

## 9. B*-Trees

來源：
- [DAC 2000 PDF](https://www.cs.york.ac.uk/rts/docs/SIGDA-Compendium-1994-2004/papers/2000/dac00/pdffiles/27_1.pdf)

可直接借：
- B*-tree 對 hard、pre-placed、soft modules 的直接操作性好
- 文中明講它對新型複雜 constraints 的處理有潛力

對本題價值：
- 如果後續要做正式 floorplan representation，B*-tree 是強選項

## 10. Are Floorplan Representations Important

來源：
- [ISPD 2005 PDF](https://web.eecs.umich.edu/~imarkov/pubs/conf/ispd05-fp.pdf)

可直接借：
- sequence pair 和 B*-tree 在不同設定下各有 tradeoff
- 不是 representation 名字越複雜就一定更好

對本題價值：
- 若只是要 contest baseline，不必太早陷入 representation 宗教戰

## 這輪 research 的結論

最值得立即轉成 code 的，不是更多 paper，而是以下 6 件事：
- 建 `fast_eval` 和 `accurate_eval` 雙層 evaluator
- 把 `constraints-aware move` 納入主 search
- 把 `channel overflow` 和 `FT overflow` 分開計分
- 允許 `bounded detour`
- 對壅塞區做 `inflation / widening / spacing`
- 保留升級到 `top-wire upper graph` 的介面

