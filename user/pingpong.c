#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

int main() {
    int p2c[2];
    int c2p[2];
    pipe(p2c);
    pipe(c2p);
    char buf[1];

    buf[0] = '~';
    
    int pid = fork();

    if(pid == 0) {
        //子进程
        //关闭两个用不到的端，防止阻塞
        close(p2c[WR]);
        close(c2p[RD]);

        //子进程接受
        if (read(p2c[RD],buf,1) != 1) {
            //子进程接受失败
            fprintf(2,"error!Child process read failed!\n");
            exit(1);
        } else {
            //子进程接受成功
            printf("%d: received ping\n",getpid());
            close(p2c[RD]);
            //子进程发送
            if (write(c2p[WR],buf,1) != 1) {
                //子进程发送失败
                fprintf(2,"error!Child process write failed!\n");
                exit(1);
            } else {
                //子进程发送成功
                close(c2p[WR]);
                exit(0);
            }
        }
    } else {
        //父进程
        close(p2c[RD]);
        close(c2p[WR]);

        //父进程发送
        if (write(p2c[WR], buf, 1) != 1) {
            //父进程发送失败
            fprintf(2,"error!Parent process write failed!\n");
            exit(1);
        } else {
            //父进程发送成功
            close(p2c[WR]);
            //父进程接受
            if (read(c2p[RD], buf, 1) != 1) {
                //父进程接受失败
                fprintf(2, "error!Parent process read failed!\n");
                exit(1);
            } else {
                //父进程接受成功
                printf("%d: received pong\n",getpid());
                close(c2p[RD]);
                exit(0);
            }
        }
    }
    




}