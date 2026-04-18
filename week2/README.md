# week2
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
