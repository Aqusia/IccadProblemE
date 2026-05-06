# Knowledge Points

## 題目級知識

- `E_2026` 是 early floorplanning 問題，不是單純 placement 或 routing 問題。
- channel 寬度與 routed net 數直接相關，題目給出 `25 nets / um`。
- soft block 可做 feedthrough，hard/edge block 不可。
- feedthrough 不是免費，會牽動 soft block 面積與整體 cost。

## 方法級知識

- 只優化 HPWL 會導致 routing hot spot。
- route-aware placement 比 placement-then-route 更適合這題。
- 可先用 pattern route / bounded detour 做快速候選搜尋。
- congestion-driven whitespace allocation 可直接轉成 channel sizing 問題。
- 合法性限制很多時，constraints-aware move 比單純 penalty 更重要。
- RUDY 類 demand estimator 很適合做內層快速篩選。
- `fast_eval + accurate_eval` 雙層 evaluator 會比只用一種 estimator 更實際。
- top wire area 可以考慮用 lower-channel graph + upper free-region graph 的雙層模型。

## 工程級知識

- 第一版不需要完整 detailed router，但一定要有可信的 demand estimator。
- 若 cost function 項目太少，solver 會把壓力推到未建模的限制上。
- 先做 deterministic initialization，再做 local search，通常比全隨機 search 更穩。
- 若迭代式優化每輪都 full reroute，runtime 很容易爆；要做局部重算。
- 若某些 bottleneck 連續多輪 overflow，可把其成本放大，逼 search 避開。
