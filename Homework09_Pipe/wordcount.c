#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int pipefd[2];
    int ret, wstat, pid1, pid2;
    // construct a pipe
    pipe(pipefd);
    pid1 = fork();
    // first child process
    if (pid1 == 0) {
        // close stdout
        close(1);
        // copy pipefd to stdout
        dup(pipefd[1]);
        // close unused pipe
        close(pipefd[1]); close(pipefd[0]);
        execl("/bin/cat", "cat", argv[1], NULL);
    }
    if (pid1 > 0) {
        pid2 = fork();
        // second child process
        if (pid2 == 0) {
            // close stdin
            close(0);
            // copy pipefd to stdin
            dup(pipefd[0]);
            // close unused pipe
            close(pipefd[1]); close(pipefd[0]);
            execl("/usr/bin/wc", "wc", argv[1], NULL);
        }
    }
    close(pipefd[0]); close(pipefd[1]);
    waitpid(pid1, &wstat, 0); waitpid(pid2, &wstat, 0);
    return 0;
}
