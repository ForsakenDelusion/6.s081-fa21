#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

void isPrime(int p[2]) {
    int char1;

    read(p[RD], &char1, sizeof(char1));
    if(char1 == -1) {
		exit(0);
	}
    printf("prime %d\n", char1);

    int pNext[2];
    pipe(pNext);

    if (fork() == 0) {
        close(p[RD]);
        close(pNext[WR]);
        isPrime(pNext);
     
    } else {
        close(pNext[RD]);
        int buf;
        while(read(p[RD], &buf, sizeof(buf)) && buf != -1) { // 从左端的进程读入数字
			if(buf % char1 != 0) { // 筛掉能被该进程筛掉的数字
				write(pNext[WR], &buf, sizeof(buf)); // 将剩余的数字写到右端进程
			}
		}
		buf = -1;
		write(pNext[1], &buf, sizeof(buf));
        wait(0);
        exit(0);
    }

}

int main(int argc, char** argv) {

    int p[2];
    pipe(p);

    if (fork() == 0) {
        close(p[WR]);
        isPrime(p);
        exit(0);
    } else {
        close(p[RD]);
        int i;
        for (i = 2; i <= 35; i++)
        {   
            write(p[WR], &i,sizeof(i));
        }
        int buf = -1;
        write(p[WR], &buf, sizeof(buf));
    
    }
    wait(0);
    exit(0);
}