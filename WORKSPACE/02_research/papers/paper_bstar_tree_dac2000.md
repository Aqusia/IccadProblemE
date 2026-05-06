# Paper: B*-Trees (DAC 2000)

原文：
- [B*-Trees: A New Representation for Non-Slicing Floorplans](https://websrv.cecs.uci.edu/~papers/compendium94-03/papers/2000/dac00/pdffiles/27_1.pdf)

## 這篇在解什麼

目標是找一種：
- 適合 non-slicing floorplans
- 評估快
- primitive tree operations 好做
- 容易整合 hard / pre-placed / soft / rectilinear modules
的 floorplan representation。

## 核心定義

B*-tree 是 ordered binary tree。
對 compact / admissible placement：
- root 對應左下角模組
- left child：放在 parent 右側、盡量往下
- right child：放在 parent 上方、x 與 parent 對齊

關鍵點：
- compact placement 和 B*-tree 之間是一對一
- placement decode 是線性時間
- 透過 horizontal contour 可以常數時間求 x，逐步更新 y

## 為什麼它重要

相對 sequence pair：
- 不用每次建 constraint graph
- 可以直接在 tree 上做操作
- insertion / deletion / search 的 primitive 動作更自然
- 表示空間較小，容易做 SA

相對 O-tree：
- 規則更整齊
- 插入位置更自然
- 對 binary-tree-based implementation 較友善

## 論文中的 SA

這篇不是只提出 representation，也直接做了 B*-tree based SA。

move set：
- rotate module
- move module to another place
- swap two modules
- remove soft module and insert it into best internal/external position

重要點：
- 不是只有 swap/rotate
- 對 soft modules 還有「抽出後找最佳插入位置」這種更強 perturb

## 對特殊模組的處理

### Pre-placed modules
- 論文強調 B*-tree 對 pre-placed modules 有彈性
- 這點後來被 PARSAC 進一步做成 anchored blocks

### Soft modules
- 固定 area、寬高可變
- 論文處理分成兩階段：
  - 先挑一個 soft module 做 delete/insert
  - 再調整其他 soft modules 的 shape

### Rectilinear modules
- 可切成多個 submodules
- 維持 left-child 關係與對齊條件

## 對你現在這題的直接意義

### 1. 用 B*-tree 當主表示法是合理的
不是只能做 PA2。
它本來就是拿來支援：
- SA
- soft module
- pre-placed / constrained placements

### 2. 不能只留最簡單 move
若只剩：
- rotate
- swap
- move subtree
會偏弱。

應該補：
- delete + best reinsert
- constraint-fixing move
- congestion-driven targeted move

### 3. block inflation 不一定是 resize actual node
對 hard macro 題目：
- 更合適的是 virtual inflation / halo
- 或把 inflation 轉成 spacing pressure

對 soft block 題目：
- 才能真的改 shape / area

## 我對這篇的結論

如果你要用 `B*-tree + SA` 當底：
- 完全合理
- 而且比 sequence pair 更貼近你現在的 repo

但要注意：
- B*-tree 只解決 representation 問題
- 不會自動解 routability
- routability / congestion / FT / channel 還是要另外建 evaluator 與 repair 機制

