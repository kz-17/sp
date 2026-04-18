# 第五章：開發工具鏈

## 5.1 工具鏈概述

### 5.1.1 什麼是開發工具鏈

開發工具鏈（Toolchain）是指從原始碼到可執行檔的整組工具集合。它包含了編譯器、組譯器、連結器、載入器等元件，共同完成將高階語言程式轉換為可在電腦上執行的機器的任務。

### 5.1.2 工具鏈的組成

```
原始碼 (.c/.cpp)
     │
     ▼
┌────────────┐    ┌───────────┐    ┌─────────┐    ┌────────────┐
│  編譯器    │───▶│  組譯器   │───▶│  連結器 │───▶│  可執行檔  │
│ (Compiler) │    │(Assembler)│    │(Linker) │    │(Executable)│
└────────────┘    └───────────┘    └─────────┘    └────────────┘
     │                                 │
     ▼                                 ▼
  目的碼 (.o)                    共享程式庫 (.so/.a/.dll)
```

### 5.1.3 常見工具鏈

| 平台 | 工具鏈 | 主要工具 |
|------|--------|----------|
| Linux/GCC | GNU Toolchain | gcc, as, ld, gdb |
| macOS | Apple LLVM | clang, llvm, lldb |
| Windows | MSVC | cl.exe, link.exe, WinDbg |

## 5.2 編譯器

### 5.2.1 編譯器的角色

編譯器（Compiler）將高階語言原始碼翻譯為低階的中間表示或目的碼。主要階段包括：
- **詞法分析（Lexical Analysis）**：識別 token
- **語法分析（Syntax Analysis）**：建立語法樹
- **語意分析（Semantic Analysis）**：類型檢查
- **最佳化（Optimization）**：提升效能
- **目的碼生成（Code Generation）**：輸出目的碼

### 5.2.2 編譯過程

```c
// 原始碼
int add(int a, int b) {
    return a + b;
}
```

經過編譯後產生目的碼（簡化示例）：
```assembly
add:
    push   rbp
    mov    rbp, rsp
    mov    eax, edi
    add    eax, esi
    pop    rbp
    ret
```

### 5.2.3 常用編譯器

#### GCC（GNU Compiler Collection）
- 支援 C、C++、Objective-C、Fortran、Go 等
- 跨平台，Linux 預設編譯器
- 指令：`gcc -o output source.c`

#### Clang/LLVM
- Apple 預設編譯器
- 編譯速度快，錯誤訊息清晰
- 指令：`clang -o output source.c`

#### MSVC（Microsoft Visual C++）
- Windows 專用
- 最佳化效果好
- 指令：`cl /Fe:output source.c`

### 5.2.4 編譯選項

```bash
# 基本編譯
gcc -c source.c          # 編譯不連結，產生 .o 檔案
gcc -o program source.c  # 編譯並連結，產生執行檔

# 警告與除錯
gcc -Wall -Wextra source.c     # 啟用所有警告
gcc -g source.c                # 產生除錯資訊
gcc -O2 source.c               # 最佳化（O0-O3, Os）

# 函式庫與Include
gcc -I /path/to/include source.c  # 指定include路徑
gcc -L /path/to/lib -lname       # 指定函式庫路徑
```

## 5.3 組譯器

### 5.3.1 組譯器的角色

組譯器（Assembler）將組譯語言翻譯為機器碼，產生目的檔案（Object File）。它是工具鏈中介於編譯器與連結器之間的元件。

### 5.3.2 組譯語言基礎

```assembly
# x86-64 Linux 組譯
.section .data
msg: .asci "Hello, World!\n"

.section .text
.globl _start
_start:
    mov rax, 1          # sys_write
    mov rdi, 1          # stdout
    lea rsi, [rel msg]  # message
    mov rdx, 14         # length
    syscall

    mov rax, 60         # sys_exit
    xor rdi, rdi        # exit code 0
    syscall
```

### 5.3.3 常見組譯器

#### NASM（Netwide Assembler）
- 語法簡潔，跨平台
- 主要用於 x86/x64
- 指令：`nasm -f elf64 source.asm -o source.o`

#### MASM（Microsoft Assembler）
- Windows 專用
- 與 Intel 語法相容

#### GAS（GNU Assembler）
- GCC 內建組譯器
- AT&T 語法

### 5.3.4 目的檔案格式

目的檔案包含：
- **符號表（Symbol Table）**：函式與變數資訊
- **重定位表（Relocation Table）**：需修正的位址
- **除錯資訊（Debug Info）**：行號、變數位置

## 5.4 連結器

### 5.4.1 連結器的角色

連結器（Linker）將一個或多個目的檔案與函式庫合併，產生最終的可執行檔或共享程式庫。主要職責：
- **符號解析（Symbol Resolution）**：解析跨檔案的參照
- **位址重定位（Relocation）**：修正相對位址
- **段合併（Section Merging）**：合併相同類型的段

### 5.4.2 連結類型

#### 靜態連結
- 所有程式碼在編譯時連結
- 產生獨立的執行檔
- 函式庫程式碼嵌入執行檔中

```bash
# 靜態連結
gcc -static -o program source.c -lpthread
```

#### 動態連結
- 程式執行時才載入函式庫
- 多個程式共享同一份函式庫
- 減少記憶體占用與執行檔大小

```bash
# 動態連結
gcc -o program source.c -lpthread
ldd program  # 檢視動態依賴
```

### 5.4.3 符號解析過程

```
檔案 A.o          檔案 B.o           輸出
┌──────────┐    ┌──────────┐     ┌──────────┐
│ main()   │    │ foo()    │     │ main()   │
│ call foo │    │          │     │ foo()    │
└──────────┘    └──────────┘     └──────────┘
   │                 │               │
   └───── 連結 ─────┘               │
         解析 foo 位址               │
```

### 5.4.4 連結器指令

```bash
# 基本連結
ld -o program a.o b.o

# 指定 entry point
ld -e main -o program a.o

# 產生共享程式庫
ld -shared -o libfoo.so foo.o

# 顯示連結腳本
ld --verbose
```

## 5.5 載入器

### 5.5.1 載入器的角色

載入器（Loader）是將可執行檔讀入記憶體並準備執行的程式。它是作業系統的一部分，在執行程式時自動運作。

### 5.5.2 載入過程

1. **開啟檔案**：讀取 ELF/PE 標頭
2. **驗證格式**：確認是有效的執行檔
3. **建立行程**：建立新的行程結構
4. **映射段**：將程式段映射到虛擬記憶體
5. **設定堆疊**：初始化堆疊空間
6. **初始化**：設定環境變數與參數
7. **跳轉執行**：設定程式計數器，開始執行

### 5.5.3 動態載入器

動態載入器（Dynamic Loader）負責載入共享程式庫：
- Linux：ld-linux.so
- macOS：dyld
- Windows：ntdll.dll

```c
// 動態載入程式庫範例
#include <dlfcn.h>

void *handle = dlopen("./libfoo.so", RTLD_LAZY);
void (*func)() = dlsym(handle, "my_function");
func();
dlclose(handle);
```

## 5.6 偵錯器

### 5.6.1 偵錯器的功能

偵錯器（Debugger）幫助開發者找出並修復程式錯誤：
- **中斷點（Breakpoint）**：暫停執行
- **逐步執行（Step）**：單步或跳過函式
- **檢視記憶體**：查看變數與記憶體內容
- **修改執行**：改變變數值
- **堆疊追蹤**：查看函式呼叫堆疊

### 5.6.2 常見偵錯器

#### GDB（GNU Debugger）
- Linux 主要偵錯器
- 指令列介面
- 支援多種語言

#### LLDB
- LLVM 專案
- macOS 預設偵錯器
- 現代化設計

#### WinDbg
- Windows 核心偵錯
- 功能強大但學習曲線高

### 5.6.3 GDB 常用指令

```bash
# 啟動
gdb ./program
gdb ./program core    # 載入 core dump

# 執行控制
run              # 開始執行
continue         # 繼續執行
next             # 單步執行（不進入函式）
step             # 單步執行（進入函式）
finish           # 執行到函式返回

# 中斷點
break function  # 在函式設中斷點
break file:line # 在檔案行號設中斷點
delete n        # 刪除中斷點
info breakpoints

# 檢視
print var       # 顯示變數值
display var     # 自動顯示變數
backtrace       # 顯示堆疊
info locals     # 顯示本地變數
x/20x addr      # 查看記憶體

# 修改
set var = 10    # 修改變數值
```

### 5.6.4 除錯資訊

編譯時加入除錯資訊：
```bash
gcc -g -o program source.c
```

使用 `strip` 移除除錯資訊：
```bash
strip -s program    # 移除符號表
```

## 5.7 其他開發工具

### 5.7.1 剖析器（Profiler）

分析程式效能，找出瓶頸：
- **gprof**：GCC 內建效能分析
- **Valgrind**：記憶體分析與效能分析
- **perf**：Linux 效能分析工具

### 5.7.2 二進位工具

| 工具 | 功能 |
|------|------|
| objdump | 反編譯、查看目標檔 |
| readelf | 查看 ELF 檔案資訊 |
| nm | 查看符號表 |
| size | 查看段大小 |
| strings | 擷取字串 |
| file | 辨識檔案類型 |

### 5.7.3 版本控制

- **Git**：分散式版本控制
- **SVN**：集中式版本控制

## 5.8 工具鏈實務

### 5.8.1 完整編譯流程

```bash
# 步驟1：編譯
gcc -c -fPIC -o a.o a.c
gcc -c -fPIC -o b.o b.c

# 步驟2：連結為共享程式庫
gcc -shared -o libfoo.so a.o b.o

# 步驟3：編譯主程式並連結
gcc -o main main.c -L. -lfoo -Wl,-rpath,.

# 步驟4：執行
./main

# 步驟5：檢視依賴
ldd ./main
```

### 5.8.2 Makefile 範例

```makefile
CC = gcc
CFLAGS = -Wall -g
TARGET = program
OBJS = main.o foo.o bar.o

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
```

## 5.9 總結

本章介紹了完整的開發工具鏈，包括編譯器、組譯器、連結器、載入器與偵錯器。理解這些工具的運作原理與使用方式，是成為系統程式開發者的基礎。下一章將深入探討語言處理器的內部實作。