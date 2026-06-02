/*
 * fork_basic.c - fork() 基本概念示範
 *
 * fork() 建立一個子行程，子行程是父行程的拷貝。
 * fork() 在父行程回傳 child PID，在子行程回傳 0。
 *
 * 編譯: gcc fork_basic.c -o fork_basic
 * 執行: ./fork_basic
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();  // 從這裡分裂成兩個行程

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // ---- 子行程 (Child Process) ----
        // fork() 回傳值為 0，代表我是子行程
        printf("[子行程] 我的 PID = %d，父行程 PID = %d\n",
               getpid(), getppid());
        printf("[子行程] fork() 回傳值 = %d (0 代表我是子行程)\n", pid);
    } else {
        // ---- 父行程 (Parent Process) ----
        // fork() 回傳值為子行程的 PID (>0)
        printf("[父行程] 我的 PID = %d\n", getpid());
        printf("[父行程] 剛建立了子行程，其 PID = %d\n", pid);
        printf("[父行程] fork() 回傳值 = %d (子行程 PID)\n", pid);

        wait(NULL);  // 等待子行程結束，避免殭屍行程
        printf("[父行程] 子行程已結束\n");
    }

    // 兩個行程都會執行到這裡
    printf("[%s] 程式結束\n", pid == 0 ? "子行程" : "父行程");
    return 0;
}
