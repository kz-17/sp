/*
 * file_io.c - open / read / write / close 示範
 *
 * 檔案描述子 (File Descriptor, FD) 是整數：
 *   0 = stdin  (標準輸入)
 *   1 = stdout (標準輸出)
 *   2 = stderr (標準錯誤輸出)
 * 開啟新檔案時，回傳的 FD 會是「當前最小可用」的整數。
 *
 * 編譯: gcc file_io.c -o file_io
 * 執行: ./file_io
 */

#include <stdio.h>
#include <fcntl.h>    // open flags: O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <unistd.h>   // read, write, close
#include <string.h>   // strlen

int main() {
    int fd;
    ssize_t bytes;
    char buffer[128];

    // ===== 1. 開啟檔案寫入 (write) =====
    // O_WRONLY: 唯寫 | O_CREAT: 不存在就建立 | O_TRUNC: 清空舊內容
    // 0644: rw-r--r-- (只有當 O_CREAT 時需要)
    fd = open("demo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }
    printf("開啟檔案 demo.txt 寫入，FD = %d\n", fd);

    char *text = "Hello, file I/O!\n第二行內容\n";
    bytes = write(fd, text, strlen(text));
    printf("寫入了 %zd bytes\n", bytes);

    close(fd);  // 關閉檔案，歸還 FD
    printf("已關閉 FD %d\n\n", fd);

    // ===== 2. 開啟檔案讀取 (read) =====
    fd = open("demo.txt", O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }
    printf("開啟檔案 demo.txt 讀取，FD = %d\n", fd);

    bytes = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes == -1) {
        perror("read failed");
        close(fd);
        return 1;
    }

    buffer[bytes] = '\0';  // 確保字串結尾
    printf("讀取了 %zd bytes:\n%s\n", bytes, buffer);

    close(fd);
    printf("已關閉 FD %d\n\n", fd);

    // ===== 3. stdin (FD 0) 讀取使用者輸入 =====
    printf("請輸入一行文字 (從 stdin 讀取): ");
    fflush(stdout);  // 確保輸出即時顯示

    bytes = read(0, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("你輸入了: %s", buffer);
    }

    // ===== 4. stdout (FD 1) 與 stderr (FD 2) =====
    write(1, "這是寫到 stdout (FD 1)\n", 24);
    write(2, "這是寫到 stderr (FD 2)\n", 24);

    // 兩者都會顯示在終端機，但可以分開重新導向：
    //   ./file_io > output.txt       (只有 stdout 進檔案)
    //   ./file_io 2> error.txt       (只有 stderr 進檔案)
    //   ./file_io > output.txt 2>&1  (兩者都進檔案)

    return 0;
}
