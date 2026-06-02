/*
 * dup_redirect.c - dup2 與 I/O 重定向示範
 *
 * dup2(old_fd, new_fd) 的作用：
 *   關閉 new_fd，然後讓 new_fd 拷貝 old_fd 的參考。
 *   從此，寫入 new_fd 等同於寫入 old_fd 的目標。
 *
 * 關鍵應用：將 stdout (1) 重新指向檔案，讓 printf 輸出到檔案。
 *
 * 編譯: gcc dup_redirect.c -o dup_redirect
 * 執行: ./dup_redirect
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int saved_stdout;

    // ===== 範例 1: 將 stdout 重新導向到檔案 =====
    printf("這行會顯示在終端機\n");

    // 先保存原來的 stdout (FD 1)，之後可以還原
    saved_stdout = dup(1);  // dup() 會拷貝 FD，回傳新的 FD
    printf("saved_stdout = %d (原 stdout 的拷貝)\n", saved_stdout);

    // 開啟（或建立）輸出檔案
    int fd = open("redirect_output.txt",
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }
    printf("新檔案 FD = %d\n", fd);

    // dup2(fd, 1) 的意義：
    //   1. 關閉 FD 1 (stdout 原本指向終端機)
    //   2. 讓 FD 1 指向 fd 所指向的檔案
    dup2(fd, 1);

    // 從這裡開始，printf 會寫入檔案！
    printf("這行被重新導向到檔案了！\n");
    printf("這裡也寫入檔案...\n");
    fprintf(stdout, "fprintf 到 stdout 也會進檔案\n");

    // 關閉原本的 fd 拷貝
    close(fd);
    // 現在只有 FD 1 還指著這個檔案

    // ===== 範例 2: 恢復 stdout =====
    // dup2(saved_stdout, 1) 將 FD 1 指回終端機
    dup2(saved_stdout, 1);
    close(saved_stdout);

    printf("這行又回到終端機了！\n\n");

    // ===== 範例 3: 將 stderr 合併到 stdout =====
    // 這相當於 shell 中的 2>&1
    printf("=== 示範 2>&1 (stderr 合併到 stdout) ===\n");
    fprintf(stderr, "此行 stderr 會單獨顯示\n");

    dup2(1, 2);  // 讓 FD 2 指向跟 FD 1 一樣的地方

    fprintf(stderr, "此行 stderr 被導向到跟 stdout 一樣的地方\n");
    write(2, "write 到 FD 2 也跟 stdout 相同\n", 32);

    printf("\n執行: cat redirect_output.txt\n");
    printf("可以看到被重新導向的輸出內容\n");

    return 0;
}
