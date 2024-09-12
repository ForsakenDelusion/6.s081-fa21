#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    char buf[1024];  // 内存池
    char* argExec[MAXARG]; // 要执行的命令和参数
    int i, n;
    int argIndex = 0;

    // 初始化argExec，把argv里的参数放入argExec
    for(i = 1; i < argc; i++) {
        argExec[argIndex++] = argv[i];
    }

    // 读取输入
    char c;
    int bufIndex = 0;
    while((n = read(0, &c, 1)) > 0) {
        if (c == '\n') {
            buf[bufIndex] = '\0';  // 将当前行设置为字符串结尾
            argExec[argIndex] = buf; // 把这一行作为新的参数
            argExec[argIndex+1] = 0; // exec参数要以NULL结尾
            
            if (fork() == 0) {  // 创建子进程
                exec(argv[1], argExec);  // 执行命令
                exit(0);  // 防止子进程意外继续执行父进程的代码
            } else {
                wait(0);  // 父进程等待子进程结束
            }

            // 重置缓冲区和索引
            bufIndex = 0;
        } else {
            buf[bufIndex++] = c;
        }
    }

    // 防御性编程，确保没有未处理的子进程。
    while(wait(0) != -1);
    
    exit(0);
}