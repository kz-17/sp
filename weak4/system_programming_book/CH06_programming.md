# 第六章：程式語言

## 6.1 系統程式語言概述

### 6.1.1 為什麼系統程式需要特定語言

系統程式設計對語言有特殊要求：
- **低層級控制**：能直接操作記憶體與硬體
- **高效率**：接近硬體的執行效能
- **可預測性**：確定性的記憶體使用與執行時間
- **硬體親和性**：能產生緊湊的機器碼

### 6.1.2 語言層級分類

```
高階語言（Python、Java、JavaScript）
    │
    ▼
中階語言（C、C++）
    │
    ▼
低階語言（組譯語言）
    │
    ▼
機器碼（0 與 1）
```

## 6.2 組譯語言

### 6.2.1 組譯語言的特性

組譯語言（Assembly Language）是最接近機器碼的語言：
- 一對一對應機器指令
- 完全控制硬體資源
- 無法移植到不同架構

### 6.2.2 x86-64 組譯基礎

```assembly
# Linux x86-64 組譯範例
.section .data
    msg: .asciz "Hello, World!\n"

.section .text
.globl _start
_start:
    # sys_write(fd=1, buf=msg, n=14)
    mov rax, 1          # system call number: write
    mov rdi, 1          # file descriptor: stdout
    lea rsi, [rel msg]  # buffer address
    mov rdx, 14         # count
    syscall

    # sys_exit(status=0)
    mov rax, 60         # system call number: exit
    xor rdi, rdi        # exit status: 0
    syscall
```

### 6.2.3 常見指令類別

| 類別 | 範例指令 | 功能 |
|------|----------|------|
| 資料移動 | mov, push, pop | 記憶體與暫存器傳輸 |
| 算術運算 | add, sub, mul, div | 數學運算 |
| 邏輯運算 | and, or, xor, not | 位元操作 |
| 控制轉移 | jmp, jne, je, call | 流程控制 |
| 比較 | cmp, test | 條件判斷 |

### 6.2.4 組合語言語法

#### Intel 語法
```asm
mov eax, ebx    ; Intel 語法
```

#### AT&T 語法
```asm
movl %ebx, %eax ; AT&T 語法
```

## 6.3 C 語言

### 6.3.1 C 語言在系統程式中的地位

C 語言是系統程式設計的主流語言：
- **低階能力**：指標運算、直接記憶體存取
- **高效能**：接近組譯的執行效率
- **可攜性**：標準化 ANSI/ISO C
- **豐富生態**：大量系統程式庫與工具

### 6.3.2 C 語言關鍵特性

```c
// 指針操作 - 直接記憶體訪問
int arr[10] = {0};
int *p = arr;
*p = 42;           // 直接寫入記憶體

// 位元操作
unsigned int flags = 0x01;
flags |= 0x04;     // 設定特定位元
flags &= ~0x02;    // 清除特定位元

// 結構體 - 低層級資料表示
struct packet {
    unsigned int header;
    unsigned int length;
    char payload[256];
};
```

### 6.3.3 C 標準庫

| 標頭檔 | 功能 |
|--------|------|
| stdio.h | 標準輸入輸出 |
| stdlib.h | 記憶體配置、工具函式 |
| string.h | 字串操作 |
| stdint.h | 固定寬度整數型態 |
| unistd.h | POSIX 系統呼叫 |

### 6.3.4 C 指標與位址

```c
// 指針基礎
int value = 100;
int *ptr = &value;     // 取地址
printf("%d\n", *ptr);  // 解引用

// 指標算術
int arr[5] = {1,2,3,4,5};
int *p = arr;
p++;                   // 移到下一個元素

// 函式指標
typedef int (*func_t)(int, int);
func_t add = &some_function;
```

## 6.4 C++

### 6.4.1 C++ 的特性

C++ 在 C 的基礎上增加了：
- **物件導向**：類別、繼承、多型
- **範型程式設計**：模板、STL
- **RAII**：資源獲取即初始化
- **例外處理**：錯誤處理機制

### 6.4.2 C++ 與系統程式

```cpp
// RAII - 自動資源管理
class FileGuard {
    FILE* f;
public:
    FileGuard(const char* path) { f = fopen(path, "r"); }
    ~FileGuard() { if (f) fclose(f); }
};

// 使用
void process() {
    FileGuard guard("data.txt");
    // 使用檔案，離開作用域自動關閉
}
```

### 6.4.3 STL 容器

```cpp
#include <vector>
#include <unordered_map>

std::vector<int> numbers = {1, 2, 3, 4, 5};
std::unordered_map<std::string, int> dict;
dict["key"] = 42;
```

## 6.5 其他系統程式語言

### 6.5.1 Rust

現代系統程式語言：
- **記憶體安全**：無空指標、無資料競爭
- **所有權系統**：編譯時記憶體管理
- **零成本抽象**：高效抽象不影響效能

```rust
fn main() {
    let s = String::from("Hello");
    let s2 = s;  // 移動語意
    // println!("{}", s);  // 錯誤！s 已移動
    println!("{}", s2);
}
```

### 6.5.2 Go

Google 設計的系統程式語言：
- **Goroutine**：輕量級執行緒
- **GC**：自動垃圾回收
- **簡單語法**：易學易用

```go
func main() {
    ch := make(chan int)
    go func() {
        ch <- 42
    }()
    fmt.Println(<-ch)
}
```

### 6.5.3 Python

高階語言但可用於系統工具：
- **豐富標準庫**：處理各種任務
- **腳本自動化**：系統管理與自動化
- **擴展模組**：可用 C 擴展效能瓶頸

```python
import os
import subprocess

# 執行系統指令
result = subprocess.run(['ls', '-la'], capture_output=True)
print(result.stdout.decode())

# 操作檔案
with open('data.txt', 'r') as f:
    content = f.read()
```

## 6.6 語言比較

### 6.6.1 系統程式語言比較

| 特性 | 組譯 | C | C++ | Rust | Go |
|------|------|---|-----|------|-----|
| 記憶體控制 | 完全 | 完全 | 完全 | 安全 | GC |
| 執行效率 | 最高 | 很高 | 很高 | 很高 | 高 |
| 學習難度 | 高 | 中 | 高 | 高 | 中 |
| 可攜性 | 低 | 高 | 高 | 高 | 高 |
| 安全性 | 低 | 低 | 中 | 高 | 中 |

### 6.6.2 語言選擇準則

- **作業系統核心**：C/組譯（Linux kernel）
- **驅動程式**：C/C++（硬體直接控制）
- **系統工具**：C/Rust/Go（效能與安全平衡）
- **腳本自動化**：Python/Bash（快速開發）

## 6.7 語言內部實現

### 6.7.1 編譯器架構

```
原始碼
   │
   ▼
┌─────────────────────┐
│   前端（Frontend）   │  詞法分析、語法分析、語意分析
└─────────┬───────────┘
          │
          ▼
┌─────────────────────┐
│   中間表示（IR）     │  平臺無關的最佳化
└─────────┬───────────┘
          │
          ▼
┌─────────────────────┐
│   後端（Backend）   │  目標機器碼生成
└─────────────────────┘
          │
          ▼
   目的碼/機器碼
```

### 6.7.2 執行期環境

- **堆疊管理**：函式呼叫與本地變數
- **堆積管理**：動態記憶體配置
- **呼叫慣例**：參數傳遞與返回值
- **例外處理**：錯誤傳播機制

## 6.8 總結

系統程式設計需要根據需求選擇合適的語言。C 語言長期以來是系統程式的主流語言，但 Rust、Go 等現代語言正在興起。理解不同語言的特性與適用場景，有助於在實際專案中做出正確的技術決策。