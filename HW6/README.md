# 行程 (Process) 與檔案 (File) 相關系統呼叫觀念釐清

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
