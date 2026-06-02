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

