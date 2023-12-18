#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print(char *s)
{
    int i = 0;
    while (s[i])
    {
        write(2, &s[i], 1);
        i++;
    }
}

void do_cd(char **cmd)
{
    if(cmd[1] == NULL || cmd[2])
    {
        print("error: cd: bad arguments\n");
    }
    else
    {
        if (chdir(cmd[1]) < 0)
        {
            print("error: cd: cannot change directory to ");
            print(cmd[1]);
            print("\n");
        }
    }
}

int main(int ac, char  **av, char **env)
{
    int i = 1;
    int j = 0;
    int pip;
    int pid;
    int n = 0;
    int fd[2];
    char *cmd[1000000] = {NULL};

    if (ac == 1)
        return 0;
    while (av[i])
    {
       pip = 0;
       j = 0;
       while (av[i] && (!strcmp(av[i], "|") || !strcmp(av[i], ";")))
            i++;
        if (av[i] == NULL)
            break;
       while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
            cmd[j++] = av[i++];
        cmd[j] = NULL;
        if (!strcmp(cmd[0], "cd"))
            do_cd(cmd);
        else
        {
            if (av[i] && !strcmp(av[i], "|"))
            {
                pip = 1;
                if (pipe(fd) < 0)
                {
                    print("error: fatal\n");
                    exit(1);
                }
            }
            if ((pid = fork()) < 0)
            {
                print("error: fatal\n");
                exit(1);
            }
            n++;
            if (pid == 0)
            {
                if (pip)
                {
                    close(1);
                    dup2(fd[1], 1);
                    close(fd[0]);
                    close(fd[1]);
                }
                if(execve(cmd[0], cmd, env) < 0)
                {
                    print("error: cannot execute ");
                    print(cmd[0]);
                    print("\n");
                }
            }
            if (pip == 0)
            {
                for(int i = 0;i < n; i++)
                    waitpid(-1,NULL, 0);
                n = 0;
            }
            if (pip)
            {
                close(0);
                dup2(fd[0], 0);
                close(fd[1]);
                close(fd[0]);
            }
        }
        if (av[i] == NULL)
            break;
        i++;
    }
    
    return (0);
}
