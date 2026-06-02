/*
 * pipe_communication.c - pipe + fork + dup2 綜合示範
 *
 * 模擬 shell 中的管線: ls -la | grep ".c" | wc -l
 *
 * pipe(int fd[2]) 建立一個管道：
 *   fd[0] 是讀取端，fd[1] 是寫入端。
 *   寫入 fd[1] 的資料可以從 fd[0] 讀出來。
 *
 * 編譯: gcc pipe_communication.c -o pipe_communication
 * 執行: ./pipe_communication
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2, pid3;

    // 建立兩個管道
    // pipe1: ls -> grep
    // pipe2: grep -> wc
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe failed");
        return 1;
    }

    // ===== 第一個子行程: ls -la =====
    pid1 = fork();
    if (pid1 == 0) {
        // 關閉不需要的 pipe 端
        close(pipe1[0]);  // 不讀取 pipe1
        close(pipe2[0]);  // 不用 pipe2 的讀取端
        close(pipe2[1]);  // 不用 pipe2 的寫入端

        // 將 stdout 導向 pipe1 的寫入端
        dup2(pipe1[1], 1);
        close(pipe1[1]);

        execlp("ls", "ls", "-la", NULL);
        perror("execlp ls failed");
        return 1;
    }

    // ===== 第二個子行程: grep \".c\" =====
    pid2 = fork();
    if (pid2 == 0) {
        // 關閉不需要的 pipe 端
        close(pipe1[1]);  // 不寫入 pipe1
        close(pipe2[0]);  // 不讀取 pipe2

        // 將 stdin 從 pipe1 的讀取端輸入
        dup2(pipe1[0], 0);
        close(pipe1[0]);

        // 將 stdout 導向 pipe2 的寫入端
        dup2(pipe2[1], 1);
        close(pipe2[1]);

        execlp("grep", "grep", "\\.c", NULL);
        perror("execlp grep failed");
        return 1;
    }

    // ===== 第三個子行程: wc -l =====
    pid3 = fork();
    if (pid3 == 0) {
        // 關閉不需要的 pipe 端
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);

        // 將 stdin 從 pipe2 的讀取端輸入
        dup2(pipe2[0], 0);
        close(pipe2[0]);

        execlp("wc", "wc", "-l", NULL);
        perror("execlp wc failed");
        return 1;
    }

    // ===== 父行程: 關閉所有 pipe 端，等待子行程 =====
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    // 等待三個子行程結束
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);

    printf("\n[父行程] 管線模擬完成！\n");
    printf("模擬的是 shell 指令: ls -la | grep \".c\" | wc -l\n");

    return 0;
}
