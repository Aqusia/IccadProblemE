# PARSAC

原文：
- [PARSAC.pdf](/mnt/d/Bili/PD/FINAL/PARSAC.pdf)

題名：
- `PARSAC: Fast, Human-quality Floorplanning for Modern SoCs with Complex Design Constraints`

重點：
- 工業級 floorplanning 不只要 area / wirelength 最小，還有 hard placement constraints。
- 單純把 constraints 丟進 SA objective 會產生很多非法或低品質解。
- 提出 constraints-aware SA（CA-SA）。
- 支援 parallel search，能產出 Pareto-optimal legal solutions front。

對本題可用：
- edge block 合法位置、outline、hard/soft block 差異都應進入 move generator。
- 很適合當 solver 主骨架參考。

可落地成：
- move 只生成合法 edge block 位置
- soft block resize 永遠維持 aspect ratio range
- hard block 永不旋轉或變形，除非題目允許

