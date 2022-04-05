#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

static int *count;
const int N = 4;
int main()
{
    count = mmap(NULL, sizeof(*count), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *count = 0;
    int child_num = N, i = 0, is_root = 0;
    pid_t pid[N];
    for (i = 0; i < N; ++i)
        pid[i] = fork();
    
    for (i = 0; i < N; ++i)
        // ischild
        if (pid[i] == 0)
            --child_num;
    // child_num == N means that the process is always a parent
    is_root = (child_num == N)? 1: 0;

    int ret = 0;
    // parent should wait until the child exit, or the result of count will be incorrect 
    while (child_num--)
        ret = wait(NULL);
	++*count;
    if (is_root)
        printf("%d\n", *count);
    return 0;
}
