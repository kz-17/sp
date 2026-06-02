/*
 * fork_exec.c - fork + execvp 示範
 *
 * fork() 建立子行程，子行程用 execvp() 替換成別的程式。
 * execvp() 會將當前行程的記憶體映像完全替換成新程式，
 * 第一個參數是檔名，第二個參數是 argv 陣列。
 *
 * 編譯: gcc fork_exec.c -o fork_exec
 * 執行: ./fork_exec
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // ---- 子行程：執行外部程式 ----
        printf("[子行程] 準備執行 'ls -la /tmp'\n\n");

        char *args[] = {"ls", "-la", "/tmp", NULL};
        //        argv[0]  argv[1]  argv[2]  終止

        execvp("ls", args);

        // 如果 execvp 失敗才會執行到這裡
        perror("execvp failed");
        return 1;
    } else {
        // ---- 父行程：等待子行程 ----
        printf("[父行程] 等待子行程 (PID=%d) 執行完畢...\n\n", pid);
        wait(NULL);
        printf("\n[父行程] 子行程已完成\n");
    }

    return 0;
}
