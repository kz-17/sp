# Thread（執行緒）概念說明

## 什麼是 Thread？

Thread（執行緒）是作業系統中能夠獨立執行的最小單位。一個程序（Process）可以包含一個或多個執行緒，這些執行緒共享同一個程序的記憶體空間和資源。

### Process vs Thread

| 特性 | Process（程序） | Thread（執行緒） |
|------|----------------|-----------------|
| 記憶體 | 獨立的位址空間 | 共享程序的記憶體空間 |
| 資源開銷 | 較大（建立、切換成本高） | 較小（建立、切換成本低） |
| 通訊方式 | 需透過 IPC（pipe、socket 等） | 可直接讀寫共用變數 |
| 獨立性 | 其中一個當掉不影響其他程序 | 一個 thread 當掉可能導致整個程序崩潰 |
| 擁有資源 | 擁有獨立的程式碼、資料、堆積 | 擁有獨立的堆疊和暫存器 |

## Thread 的優點

1. **高效率通訊**：同一程序內的 threads 共享記憶體，不需要複雜的 IPC 機制
2. **低開銷**：建立和切換 thread 的成本遠低於 process
3. **提高 CPU 使用率**：多執行緒可以在多核心 CPU 上平行執行
4. **提升響應性**：即使部分 thread 被阻塞，其他 thread 仍可繼續執行

## Thread 同步問題

當多個 threads 同時存取共享資源時，會產生「競爭條件」（Race Condition）。為了解決這個問題，需要使用同步機制：

### 競爭條件（Race Condition）

當多個 threads 同時讀寫同一變數，且執行順序不確定時，最終結果會取決於 threads 的執行順序，可能導致錯誤。例如：

```
Thread A 讀取 balance = 1000
Thread B 讀取 balance = 1000
Thread A 計算 balance + 500 = 1500
Thread B 計算 balance - 300 = 700   // 寫入的是舊值！
Thread A 寫入 balance = 1500        // 覆蓋了 B 的結果！
// 最終 balance = 1500（錯誤，應為 1200）
```

### 臨界區段（Critical Section）

臨界區段是指存取共享資源的程式碼區塊。在同一時間只能有一個 thread 進入臨界區段。

### Mutex（互斥鎖）

Mutex 是最基本的同步工具，用來保護臨界區段：

- `pthread_mutex_lock(&mutex)`：取得鎖，如果鎖已被佔用則阻塞等待
- `pthread_mutex_unlock(&mutex)`：釋放鎖

### Semaphore（信號量）

Semaphore 是一個計數器，用來控制同時存取資源的 thread 數量：

- `sem_wait(&sem)`：P 操作，將 semaphore 減 1，若為負數則阻塞
- `sem_post(&sem)`：V 操作，將 semaphore 加 1，喚醒等待的 thread

### Condition Variable（條件變數）

Condition variable 讓 thread 在特定條件成立之前進入等待狀態：

- `pthread_cond_wait()`：釋放 mutex 並等待條件成立
- `pthread_cond_signal()`：喚醒一個等待的 thread

## 經典同步問題

### 1. 銀行存提款問題（Race Condition）

模擬多個執行緒同時對同一銀行帳戶進行存款和提款。若無同步機制，最終餘額會因為 Race Condition 而出錯。解決方式：使用 mutex 保護帳戶餘額。

### 2. 生產者消費者問題（Producer-Consumer）

生產者將資料放入緩衝區，消費者從緩衝區取出資料。需要解決：
- 緩衝區滿時生產者等待
- 緩衝區空時消費者等待
- 避免競爭條件

### 3. 哲學家用餐問題（Dining Philosophers）

五位哲學家坐在圓桌前，每人左右各有一根筷子。哲學家需要同時拿起左右兩根筷子才能吃飯。需要解決：
- Deadlock（死結）：所有哲學家同時拿起左邊筷子，導致無人能吃飯
- Starvation（饑餓）：某些哲學家永遠吃不到飯
