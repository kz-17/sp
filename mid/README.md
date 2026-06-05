# CPU 排程器模擬

## 專案簡介

本專案實作了六種經典的 CPU 排程演算法模擬，並比較其效能指標。透過模擬不同排程策略下的行程執行順序，計算並分析各演算法的 **周轉時間 (Turnaround Time)**、**等待時間 (Waiting Time)** 與 **回應時間 (Response Time)**。

## 實作演算法

| 演算法 | 縮寫 | 類型 |
|--------|------|------|
| First-Come, First-Served | FCFS | 非搶先式 |
| Shortest Job First | SJF | 非搶先式 |
| Shortest Remaining Time First | SRTF | 搶先式 |
| Round Robin | RR | 搶先式 (with time quantum) |
| Priority (Non-Preemptive) | Priority-NP | 非搶先式 |
| Priority (Preemptive) | Priority-P | 搶先式 |

## 演算法說明

### FCFS (First-Come, First-Served)
依照行程到達順序進行排程。最簡單的排程演算法，但可能產生「護航效應」(Convoy Effect)，即短行程被長行程阻塞。

### SJF (Shortest Job First)
在每一次排程決策時，選擇所有已到達行程中 CPU 爆裂時間 (Burst Time) 最短者執行，直到該行程完成。可最小化平均等待時間，但需預先知道爆裂時間。

### SRTF (Shortest Remaining Time First)
SJF 的搶先式版本。在每個時間單元結束時，若有新行程到達，則重新選擇剩餘時間最短的行程執行。可進一步降低平均等待時間，但上下文切換次數較多。

### Round Robin (RR)
每個行程依序獲得固定長度的時間配額 (Time Quantum)，時間到則強制切換給佇列中的下一個行程。時間配額的選擇至關重要：太大則退化成 FCFS，太小則上下文切換負擔過重。

### Priority 排程 (Non-Preemptive)
選擇所有已到達行程中優先級最高者（數字越小優先級越高）執行，直到完成。低優先級行程可能面臨「饑餓」(Starvation) 問題。

### Priority 排程 (Preemptive)
Priority 的搶先式版本。在每個時間單元結束時，若有新行程到達且優先級高於當前執行行程，則立即切換。

## 效能指標定義

- **周轉時間 (Turnaround Time, TAT)** = 完成時間 − 到達時間
- **等待時間 (Waiting Time)** = 周轉時間 − CPU 爆裂時間
- **回應時間 (Response Time)** = 首次獲得 CPU 的時間 − 到達時間

## 使用方法

### 環境需求

- Python 3.6+

### 輸入格式

建立 `processes.txt`，每行一個行程：

```
# PID  Arrival  Burst  Priority
P1    0        10     3
P2    1        5      1
P3    3        2      4
P4    5        8      2
P5    6        3      5
```

各欄位意義：
- `PID` — 行程識別碼
- `Arrival` — 到達時間
- `Burst` — CPU 爆裂時間
- `Priority` — 優先級（數字越小優先級越高）

### 執行

```bash
# 執行所有演算法
python scheduler_sim.py processes.txt

# 指定時間配額（預設 2）
python scheduler_sim.py processes.txt -q 3

# 只執行特定演算法
python scheduler_sim.py processes.txt -a fcfs sjf rr
python scheduler_sim.py processes.txt -a srtf priority-p
```

### 參數一覽

| 參數 | 說明 |
|------|------|
| `input` | 輸入檔案路徑（預設 processes.txt）|
| `-q, --quantum` | Round Robin 時間配額（預設 2）|
| `-a, --algorithms` | 指定演算法，可複選：fcfs, sjf, srtf, rr, priority-np, priority-p, all（預設 all）|

### 輸出範例

```
======================================================================
  SRTF (Preemptive)
======================================================================
+-------+----------+---------+------------+---------+---------+---------+--------------+----------+
|  PID  | Arrival  |  Burst  |  Priority  |  Start  |  Finish  |  Resp.  |  Turnaround  |  Waiting  |
+-------+----------+---------+------------+---------+---------+---------+--------------+----------+
|  P1   |    0     |   10    |     3      |    0    |    17    |    0    |     17       |    7     |
|  P2   |    1     |   5     |     1      |    1    |    6     |    0    |     5        |    0     |
|  P3   |    3     |   2     |     4      |    6    |    8     |    3    |     5        |    3     |
|  P4   |    5     |   8     |     2      |    8    |    16    |    3    |     11       |    3     |
|  P5   |    6     |   3     |     5      |   16    |    19    |   10    |     13       |    10    |
+-------+----------+---------+------------+---------+---------+---------+--------------+----------+

  Avg Turnaround: 10.20
  Avg Waiting:    4.60
  Avg Response:   3.20
```

以及最終的摘要比較表：

```
======================================================================
  SUMMARY COMPARISON
======================================================================
+--------------------------+-----------+--------------+---------------+
|       Algorithm          |  Avg TAT  |  Avg Waiting  |  Avg Response  |
+--------------------------+-----------+--------------+---------------+
|         FCFS             |  13.60    |    8.00       |     7.00      |
|  SJF (Non-Preemptive)    |  11.20    |    5.60       |     4.60      |
|    SRTF (Preemptive)     |  10.20    |    4.60       |     3.20      |
|      Round Robin         |  12.80    |    7.20       |     1.60      |
| Priority (Non-Preemptive)|  15.60    |   10.00       |     9.00      |
|   Priority (Preemptive)  |  13.60    |    8.00       |     4.20      |
+--------------------------+-----------+--------------+---------------+
```

## 程式架構

```
mid/
├── scheduler_sim.py   # 主程式
├── processes.txt      # 範例輸入檔
└── README.md          # 說明文件
```

### 程式模組

| 函式 | 說明 |
|------|------|
| `Process` | 行程類別，記錄到達時間、爆裂時間、優先級及排程指標 |
| `simulate_fcfs()` | FCFS 模擬 |
| `simulate_sjf()` | SJF 模擬 |
| `simulate_srtf()` | SRTF 模擬 |
| `simulate_rr()` | Round Robin 模擬 |
| `simulate_priority_np()` | Priority 非搶先式模擬 |
| `simulate_priority_p()` | Priority 搶先式模擬 |
| `print_results()` | 輸出單一演算法結果表格 |
| `print_summary()` | 輸出所有演算法比較摘要 |
| `parse_input()` | 解析輸入檔 |
| `main()` | 進入點，處理命令列參數 |

## 實驗與分析

### 情境一：不同時間配額對 Round Robin 的影響

使用預設範例數據，分別以 quantum = 1, 2, 4, 8 測試：

| Quantum | Avg TAT | Avg Waiting | Avg Response |
|---------|---------|-------------|--------------|
| 1       | 14.20   | 8.60        | 0.80         |
| 2       | 12.80   | 7.20        | 1.60         |
| 4       | 11.80   | 6.20        | 3.00         |
| 8       | 13.00   | 7.40        | 5.00         |

- Quantum 越小，回應時間越短（互動性越好），但周轉時間較長（上下文切換次數多）。
- Quantum 過大時趨近 FCFS。

### 情境二：搶先式 vs 非搶先式

- SRTF 的平均等待時間優於 SJF（因 SRTF 能更早處理短行程）。
- Priority Preemptive 的平均回應時間優於 Non-Preemptive（高優先級行程可立即搶佔 CPU）。

## 結論

1. **SRTF** 在平均等待時間與周轉時間方面表現最佳。
2. **Round Robin** 在回應時間方面表現最佳，適合互動式系統。
3. **FCFS** 實作最簡單，但效能最差，易受護航效應影響。
4. **Priority 排程** 可能導致低優先級行程饑餓，可搭配 Aging 機制改善。
5. 排程演算法的選擇需在 **吞吐量、回應時間、公平性** 之間取得平衡。

## 參考資料

- Silberschatz, A., Galvin, P. B., & Gagne, G. *Operating System Concepts*, 10th Edition.
- Tanenbaum, A. S. *Modern Operating Systems*, 4th Edition.
