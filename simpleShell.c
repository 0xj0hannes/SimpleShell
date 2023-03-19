#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int get_arg(char *c, char *arg[], char *sym);
int get_path(char *dir[]);

int main()
{
	pid_t pid;
	int i, ret, ret2, st, flag, fd[2];
	char line[256], cmd[256], path[512];
	char *arg[32], *arg_c1[32], *arg_c2[32], *dir[64];

	get_path(dir);

	while (1)
	{
		printf("$ ");
		fgets(line, sizeof(line), stdin);
		sscanf(line, "%[^\n]", cmd);

		flag = get_arg(cmd, arg, "|");
		if (flag == 1)
		{
			get_arg(arg[0], arg_c1, " ");

			if (!strcmp("exit", arg_c1[0]))
				exit(0);

			pid = fork();
			if (pid == 0)
			{
				for (i = 0; dir[i] != NULL; i++)
				{
					sprintf(path, "%s/%s", dir[i], arg_c1[0]);
					ret = execv(path, arg_c1);
				}
				if (ret < 0)
				{
					exit(0);
				}
			}
			else
			{
				wait(&st);
			}
		}
		else if(flag == 2)
		{
			get_arg(arg[0], arg_c1, " ");
			get_arg(arg[1], arg_c2, " ");

			if(pipe(fd) < 0)
			{
				perror("pipe");
				exit(0);
			}

			pid = fork();
			if(pid == 0)
			{
				close(fd[0]);
				dup2(4, 1);
				for (i = 0; dir[i] != NULL; i++)
				{
					sprintf(path, "%s/%s", dir[i], arg_c1[0]);
					ret = execv(path, arg_c1);
				}
				if (ret < 0)
				{
					exit(0);
				}
			}

			pid = fork();
			if(pid == 0)
			{
				close(fd[1]);
				dup2(3, 0);
				for(i = 0; dir[i] != NULL; i++)
				{
					sprintf(path, "%s/%s", dir[i], arg_c2[0]);
					ret2 = execv(path, arg_c2);
				}
				if(ret2 < 0)
				{
					exit(0);
				}
			}

			close(fd[0]);
			close(fd[1]);
			wait(&st);
			wait(&st);
		}
	}
	return 0;
}

int get_arg(char *c, char *arg[], char *sym)
{
	int i = 0;
	arg[i] = strtok(c, sym);
	while (arg[i] != NULL)
	{
		i++;
		arg[i] = strtok(NULL, sym);
	}
	return i;
}

int get_path(char *dir[])
{
	char *env;
	int i = 0;
	env = getenv("PATH");
	dir[i] = strtok(env, ":");
	while (dir[i] != NULL)
	{
		i++;
		dir[i] = strtok(NULL, ":");
	}
	return i;
}