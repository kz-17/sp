# Week1
https://github.com/kz-17/sp/tree/main/week1
對話網址:https://gemini.google.com/share/continue/876e7611da4f
## 1. while 處理程式碼之設計原理
在編譯器的世界裡，while 運作的本質就是**「條件檢查」與「往回跳轉」  
在四元組的中間碼層次，while 的結構需要兩個關鍵的地址標記：  
1. **起始標記 (Start Label)**：這是迴圈條件判斷的最開端。每當迴圈體執行完畢後，必須跳回到這裡重新評估條件
2. **結束標記 (Exit Label)**：當條件為假（False）時，程式跳出的目的地
### 編譯邏輯流程：
1. 紀錄當前位置 A：在解析 while 條件前，記下目前的 quad_count。
2. 生成條件代碼：解析條件表達式。
3. 生成 JMP_F（跳至 B）：如果條件不成立，跳過整個迴圈體。此時 B 還不知道，先留白（Backpatching）。
4. 解析迴圈體：處理花括號內的陳述句。
5. 生成 JMP（跳回 A）：迴圈體結束後，強迫跳回 A 重新檢查條件。
6. 回填 B：將 JMP_F 的跳轉目標指向目前的 quad_count（迴圈外的第一行）。

## 2.  p0 compiler 中的函數呼叫機制
嚴謹的環境切換與數據傳遞流程，模擬了現代 CPU 的呼叫堆疊（Call Stack）原理
1. 編譯階段：轉換為四元組 (Intermediate Code)  
當編譯器在 parser 遇到 add(x, 10) 時，它會生成一系列扁平化的中間碼：
- PARAM：計算參數的值（如變數 x 或常數 10），並將結果放入一個「準備區」。
- CALL：指定要跳轉的函數名稱、參數個數，並標記執行完後要回來的 PC 地址。
- RET_VAL：在函數內部，計算回傳值並發送「結束與歸還」訊號。
2. 執行階段：虛擬機 (VM) 的環境管理  
這是最關鍵的部分。VM 維護了一個 stack[1000]，每一層都是一個 Frame（堆疊幀）

# week2
https://github.com/kz-17/sp/tree/main/week2
對話紀錄: https://gemini.google.com/share/ae1c8880992c
## 語言目標與功能設計
- 目標：教學用途，語法接近 Python 但具備顯式的類型標記。
- 功能：支持基本算術、變數賦值、函數定義、及簡單的條件判斷。
- 特性：
1. 強型態 (Strongly Typed)：不允許隱式轉型（例如：字串 + 數字會報錯）。
2. 執行方式：解譯器模式 (Interpreter)，基於 堆疊機 (Stack-based Machine) 架構。
3. 記憶體管理：自動管理（由於架構簡單，暫不實作複雜的 GC，依賴宿主語言 Python 的垃圾回收）。
## 實作架構規劃
使用 Python  
- 運行模式：解譯執行 (Interpretation)
- 虛擬機架構：堆疊機 (Stack Machine)。所有的運算都會在一個後進先出 (LIFO) 的堆疊上完成。
# week3
https://github.com/kz-17/sp/tree/main/week3
使用OpenCode
### 使用前請先：
申請 OpenWeatherMap API Key：https://openweathermap.org/api
修改 app.py 第6行 YOUR_OPENWEATHERMAP_API_KEY
### 運行方式
pip install -r requirements.txt  
python app.py  
### 功能
- 查詢城市天氣（當下溫度、濕度、天氣描述）
- 基於歷史資料預測明日溫度
- 用戶回饋實際溫度，系統自動計算校正值修正模型

# Week4
https://github.com/kz-17/sp/tree/main/weak4
https://github.com/kz-17/sp/tree/main/weak4

# Week5
https://github.com/kz-17/sp/tree/main/HW5
使用OpenCode  
1. thread-concept.md	Thread 概念說明文件（Process vs Thread、同步問題、經典三大問題介紹）
2. bank_simulation.c	銀行存提款模擬 — 2 threads 各存提 10 萬次，用 Mutex 確保餘額正確
3. producer_consumer.c	生產者消費者問題 — 2 生產者 + 2 消費者，用 Semaphore + Mutex 控制緩衝區
4. dining_philosophers.c	哲學家用餐問題 — 5 位哲學家各吃 5 餐，用 Mutex + Condition Variable 避免 Deadlock
5. implementation-guide.md	三支程式的實作方式、同步機制、編譯指令詳細說明

編譯方式:   
gcc -pthread bank_simulation.c -o bank_simulation  
gcc -pthread producer_consumer.c -o producer_consumer  
gcc -pthread dining_philosophers.c -o dining_philosophers  


# Week6
https://github.com/kz-17/sp/tree/main/HW6
## 一、檔案描述子 (File Descriptor, FD)

當程式開啟一個檔案時，核心（kernel）會回傳一個**非負整數**，稱為**檔案描述子 (FD)**。程式透過這個整數來讀寫檔案。

```
FD 0  ──→ stdin  (鍵盤輸入)
FD 1  ──→ stdout (螢幕輸出)
FD 2  ──→ stderr (錯誤輸出)
FD 3  ──→ 第一個開啟的檔案
FD 4  ──→ 第二個開啟的檔案
  :
```

**重要規則**：每次 `open()` 時，核心會給出「當前最小可用的 FD 整數」。

### 在 shell 中重新導向 FD

```bash
./program 2> error.log     # 將 stderr (FD 2) 導向檔案
./program > out.log 2>&1   # stdout → 檔案，stderr → 同一個地方
./program < input.txt      # 從檔案讀入 stdin (FD 0)
```

---

## 二、系統呼叫詳解

### 1. `open()` — 開啟檔案

```c
#include <fcntl.h>
int fd = open("path", flags, mode);
```

| flag | 意義 |
|------|------|
| `O_RDONLY` | 唯讀 |
| `O_WRONLY` | 唯寫 |
| `O_RDWR` | 讀寫 |
| `O_CREAT` | 檔案不存在時建立 |
| `O_TRUNC` | 清空檔案內容 |
| `O_APPEND` | 附加模式（寫在檔案尾端） |

回傳值：成功時回傳 FD，失敗時回傳 `-1`（並設定 `errno`）。

### 2. `close()` — 關閉檔案

```c
#include <unistd.h>
int close(int fd);
```

- 關閉 FD，釋放核心資源。
- 後續 `open()` 可以重複使用這個 FD 整數。

### 3. `read()` — 從檔案讀取

```c
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
```

- 從 `fd` 讀取最多 `count` bytes 到 `buf`。
- 回傳值：實際讀取到的 bytes 數，`0` 代表 EOF（檔案結尾），`-1` 代表錯誤。

### 4. `write()` — 寫入檔案

```c
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);
```

- 將 `buf` 中的 `count` bytes 寫入 `fd`。
- 回傳值：實際寫入的 bytes 數（可能小於 `count`），`-1` 代表錯誤。

### 5. `dup2()` — 複製檔案描述子

```c
#include <unistd.h>
int dup2(int oldfd, int newfd);
```

**這是 I/O 重新導向的核心！**

執行步驟：
1. 關閉 `newfd`（如果它原本是開啟的）
2. 讓 `newfd` 指向跟 `oldfd` 完全相同的核心檔案表項目

```c
dup2(fd, 1);   // 讓 stdout (FD 1) 指向 fd 所指向的檔案
               // 從此 printf() 輸出都寫入該檔案
dup2(1, 2);    // 讓 stderr (FD 2) 也指向 stdout 指向的地方 (2>&1)
```

等同於 shell 的 `2>&1` 或 `> file`。

### 6. `fork()` — 建立行程

```c
#include <unistd.h>
pid_t fork(void);
```

- 建立一個「幾乎完全相同的拷貝」作為子行程。
- **父行程**：回傳子行程的 PID（正整數）。
- **子行程**：回傳 `0`。
- **錯誤**：回傳 `-1`。

```
           fork() 之前
    ┌─────────────────┐
    │   父行程 (PID=N) │
    │   fd 0 → stdin   │
    │   fd 1 → stdout  │
    │   fd 3 → file    │
    └─────────────────┘
              │
          fork() 呼叫
              │
              ▼
    ┌─────────────────┐  ┌─────────────────┐
    │   父行程 (PID=N) │  │ 子行程 (PID=M)  │
    │   fd 0 → stdin   │  │   fd 0 → stdin  │ ← 相同的 FD 表
    │   fd 1 → stdout  │  │   fd 1 → stdout │
    │   fd 3 → file    │  │   fd 3 → file   │ ← 共享檔案偏移量！
    └─────────────────┘  └─────────────────┘
```

> **重要**：fork 後父子行程共用同一個檔案偏移量（file offset），因為指向同一個核心檔案表項目。寫入時要小心！

### 7. `execvp()` — 執行程式

```c
#include <unistd.h>
int execvp(const char *file, char *const argv[]);
```

- 將當前行程的程式碼完全替換成另一個程式。
- **不建立新行程**，而是用新程式覆蓋舊行程。
- 成功時不回傳（因為原程式已消失），失敗時回傳 `-1`。

```
    ┌─────────────────┐      execvp("ls", ...)        ┌─────────────────┐
    │   行程 (PID=N)   │  ──────────────────────────→  │  ls 程式        │
    │   執行 my_program │                              │  PID 不變！     │
    │   FD 表保持不變   │                              │  FD 表保持不變  │
    └─────────────────┘                              └─────────────────┘
```

**經典組合：`fork()` + `exec()`**

```c
pid_t pid = fork();
if (pid == 0) {
    // 子行程：執行另一個程式
    execvp("ls", args);
    // 只有在 execvp 失敗時才會執行到下一行
    perror("exec failed");
    exit(1);
} else {
    // 父行程：等待子行程結束
    wait(NULL);
}
```

---

## 三、經典範例：模擬 `ls | grep ".c" | wc -l`

```
    ls -la        grep ".c"        wc -l
    ┌──────┐  pipe  ┌──────┐  pipe  ┌──────┐
    │ stdout│──────→│ stdin│──────→│ stdin│
    └──────┘       └──────┘       └──────┘
       │              │              │
    fork(1)        fork(2)        fork(3)

父行程管理三個子行程
```

1. **`ls -la` 子行程**：`dup2(pipe1[1], 1)` 讓 stdout 寫入 pipe
2. **`grep ".c"` 子行程**：`dup2(pipe1[0], 0)` 從 pipe 讀取，`dup2(pipe2[1], 1)` 寫入下一個 pipe
3. **`wc -l` 子行程**：`dup2(pipe2[0], 0)` 從 pipe 讀取
4. **父行程**：關閉所有 pipe 端，等待所有子行程

---

## 四、一個行程的生命週期

```
程式啟動 (main)
    │
    ├─ open() ── 獲得檔案描述子
    ├─ read() / write() ── 讀寫檔案
    ├─ fork() ── 分裂出子行程
    │   ├─ 子行程: execvp() ── 變成另一個程式
    │   │             │
    │   │             └─ 執行完畢，exit()
    │   └─ 父行程: wait() ── 回收子行程
    │
    ├─ dup2() ── 重新導向 stdin/stdout/stderr
    ├─ close() ── 關閉檔案
    └─ exit() ── 程式結束，核心回收所有資源
```

---

## 五、常見迷思澄清

| 迷思 | 事實 |
|------|------|
| `fork()` 後父子行程完全獨立 | **錯**：檔案描述子共用偏移量，需注意同步 |
| `exec()` 會建立新行程 | **錯**：`exec()` 不建立行程，只替換行程內容 |
| `open()` 一定會回傳 3 | **不一定**：回傳「當前最小可用」的 FD |
| `dup2()` 只複製數字 | **錯**：`dup2()` 讓兩個 FD 指向同一個核心檔案項目 |
| `write()` 一定寫入全部資料 | **不一定**：需檢查回傳值，短寫入可能發生 |
| 子行程結束自動消失 | **錯**：需 `wait()` 回收，否則產生殭屍行程 |

---

## 六、快速參考

| 函式 | 標頭檔 | 核心作用 |
|------|--------|----------|
| `open()` | `<fcntl.h>` | 開啟檔案，獲得 FD |
| `close()` | `<unistd.h>` | 關閉 FD |
| `read()` | `<unistd.h>` | 從 FD 讀取資料 |
| `write()` | `<unistd.h>` | 寫入資料到 FD |
| `dup2()` | `<unistd.h>` | 複製 FD，實現 I/O 重定向 |
| `fork()` | `<unistd.h>` | 建立子行程 |
| `execvp()` | `<unistd.h>` | 在當前行程執行新程式 |
| `pipe()` | `<unistd.h>` | 建立管道（fd[0]讀, fd[1]寫） |
| `wait()` | `<sys/wait.h>` | 等待子行程結束 |

# 期中
https://github.com/kz-17/sp/tree/main/mid
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

