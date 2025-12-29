#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int run_command(const char *cmd) {
    if (!cmd) {
        fprintf(stderr, "Error: Invalid command\n");
        return 1;
    }

    printf("Executing: %s\n", cmd);
    fflush(stdout);

    int ret = system(cmd);
    if (ret == -1) {
        fprintf(stderr, "Failed to execute command '%s': %s\n", cmd, strerror(errno));
        return 1;
    }

    if (WIFEXITED(ret) && WEXITSTATUS(ret) != 0) {
        fprintf(stderr, "Command '%s' exited with status %d\n", cmd, WEXITSTATUS(ret));
        return WEXITSTATUS(ret);
    }

    if (WIFSIGNALED(ret)) {
        fprintf(stderr, "Command '%s' terminated by signal %d\n", cmd, WTERMSIG(ret));
        return 1;
    }

    return 0;
}
