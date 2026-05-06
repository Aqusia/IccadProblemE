# System Overview

預期 solver 可拆成：
- `parser`
- `data_model`
- `initial_floorplan`
- `routing_estimator`
- `channel_sizer`
- `feedthrough_allocator`
- `cost_evaluator`
- `optimizer`
- `writer_cfg`

高層流程：
1. 讀 testcase
2. 建 block / net / constraints model
3. 產生合法初始 floorplan
4. 做 block-level routing estimate
5. 計算 channel demand / FT demand
6. 更新 block size / channel geometry
7. 計算 cost
8. 迭代改善
9. 輸出 `.cfg`

