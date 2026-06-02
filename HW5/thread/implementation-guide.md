# 程式實作說明

## 一、銀行存提款模擬（bank_simulation.c）

### 問題描述
兩個執行緒對同一個銀行帳戶進行多次存款和提款，每次先存 100 元再提 100 元。
若無同步機制，因為 Race Condition，最終餘額會偏離初始值。

### 實作方式

- **初始餘額**：1,000,000 元
- **執行緒數量**：2 個
- **每個執行緒操作次數**：100,000 次（每次先存 100 再提 100）
- **總操作次數**：200,000 次（存入 100,000 次 + 提取 100,000 次）

### 同步機制：Mutex

```c
void deposit(BankAccount *acc, int amount) {
    pthread_mutex_lock(&acc->lock);
    acc->balance += amount;
    pthread_mutex_unlock(&acc->lock);
}

void withdraw(BankAccount *acc, int amount) {
    pthread_mutex_lock(&acc->lock);
    acc->balance -= amount;
    pthread_mutex_unlock(&acc->lock);
}
```

每次對餘額讀寫時都先取得 mutex 鎖，確保同一時間只有一個 thread 能存取餘額，避免 Race Condition。

### 驗證方式
程式會輸出最終餘額，若正確則應與初始值相同（1,000,000 元）。
若故意移除 mutex 鎖，會觀察到最終餘額錯誤。

---

## 二、生產者消費者問題（producer_consumer.c）

### 問題描述
生產者執行緒產生資料放入有限大小的緩衝區，消費者執行緒從緩衝區取出資料。
需要解決：
1. 緩衝區滿時生產者須等待
2. 緩衝區空時消費者須等待
3. 多執行緒同時存取緩衝區的 Race Condition

### 實作方式

- **緩衝區大小**：5 個 slots
- **生產者**：2 個，各產生 20 個項目
- **消費者**：2 個，各消費 20 個項目

### 同步機制：Semaphore + Mutex

使用三個同步工具：

| 工具 | 用途 | 初始值 |
|------|------|--------|
| `empty` semaphore | 計算剩餘空位 | BUFFER_SIZE = 5 |
| `full` semaphore | 計算已佔用位置 | 0 |
| `mutex` | 保護緩衝區的讀寫 | - |

- **生產者流程**：`sem_wait(empty)` → `lock(mutex)` → 放入資料 → `unlock(mutex)` → `sem_post(full)`
- **消費者流程**：`sem_wait(full)` → `lock(mutex)` → 取出資料 → `unlock(mutex)` → `sem_post(empty)`

Semaphore 確保滿/空時的阻塞行為，Mutex 確保緩衝區操作的原子性。

---

## 三、哲學家用餐問題（dining_philosophers.c）

### 問題描述
五位哲學家坐在圓桌前，每人左右各有一根筷子。
哲學家需要同時拿起左右兩根筷子才能吃飯。
每次只能有一人使用一根筷子。

### 潛在問題

| 問題 | 說明 |
|------|------|
| **Deadlock** | 所有哲學家同時拿起左邊筷子，無人能吃飯 |
| **Starvation** | 某位哲學家永遠拿不到筷子 |

### 實作方式：資源階層 + Condition Variable

使用 **Dijkstra 的解法**，關鍵在於：

1. **狀態管理**：每位哲學家有三種狀態：THINKING、HUNGRY、EATING
2. **條件檢查**：哲學家只有在「左右鄰居都不在用餐」時才能開始吃
3. **Condition Variable**：若無法用餐則等待；當鄰居放下筷子時會檢查是否能喚醒等待者

```c
void test(int i) {
    if (state[i] == HUNGRY &&
        state[left(i)] != EATING &&
        state[right(i)] != EATING) {
        state[i] = EATING;
        pthread_cond_signal(&cond[i]);
    }
}
```

### 避免 Deadlock 的關鍵
- 哲學家不會同時持有筷子等待（不像傳統的鎖左邊再鎖右邊）
- 透過 condition variable 機制，哲學家只有在**能夠同時拿到兩根筷子**時才會開始用餐
- 任何時候只要有至少一位哲學家能吃飯，系統就會持續前進
- 每位哲學家吃完規定的餐數（5 餐）後就會離開

---

## 編譯與執行

### Linux / macOS（使用 gcc）
```bash
# 銀行存提款模擬
gcc -pthread bank_simulation.c -o bank_simulation
./bank_simulation

# 生產者消費者問題
gcc -pthread producer_consumer.c -o producer_consumer
./producer_consumer

# 哲學家用餐問題
gcc -pthread dining_philosophers.c -o dining_philosophers
./dining_philosophers
```

### Windows（使用 MinGW）
```bash
gcc bank_simulation.c -o bank_simulation.exe -lpthread
bank_simulation.exe
```

> 注意：Windows 上可能需要安裝 pthread-win32 或使用 WSL（Windows Subsystem for Linux）來編譯執行。

---

## 總結

| 問題 | 主要同步機制 | 解決目標 |
|------|------------|----------|
| 銀行存提款 | Mutex | 避免 Race Condition，確保餘額正確 |
| 生產者消費者 | Semaphore + Mutex | 緩衝區滿/空時阻塞，避免競爭條件 |
| 哲學家用餐 | Mutex + Condition Variable | 避免 Deadlock 和 Starvation |
