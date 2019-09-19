
#include <stdio.h>
#include <unistd.h>
#include "get_next_line/get_next_line.h"
#include <signal.h>
#include <termios.h>

int job = 0;
int bjob = 0;

struct termios term;
struct termios term_child;

void    ft_excute(char *cmd)
{
	char **tab;
	int pid;
	int status;

	tab = ft_strsplit(cmd, ' ');
	if (!(pid = fork()))
	{
		signal(SIGTTIN, SIG_DFL);
		signal(SIGTTOU, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		pid = getpgrp();
		setpgid(pid, pid);
		execvp(tab[0], tab);
		ft_putendl("ERROR");
		exit(0);
	}
	else
	{
		setpgid(pid, pid);
		if (tcsetpgrp(0, pid) == -1)
			ft_putendl("ERROR in seting the controling terminal to the child process");
		waitpid(pid, &status, WUNTRACED);
		job = pid;
		if (WIFSTOPPED(status))
		{
			job = pid;
			tcgetattr(0, &term_child);
		}
		if (tcsetpgrp(0, getpgrp()) == -1)
				ft_putendl("ERROR in reset the controling terminal to the parent process");
		tcsetattr(0, TCSANOW, &term);
	}
}

void    ft_catch_child(int sig)
{
	int status;
	int pid;

	printf("job == %d \n", job);
 	if (waitpid(job, &status, WNOHANG))
 	{
		if (WIFEXITED(status))
			printf("[1] done  job == %d\n", job);
 	}

 }

int main()
{
	char *line;
	int status;

	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGCHLD, ft_catch_child);
	tcgetattr(0, &term);
	while (1)
	{
		ft_putstr("$> ");
		if (get_next_line(0, &line))
		{
			if (ft_strequ(line, "exit"))
				exit(0);
			else if (ft_strequ(line, "fg"))
			{
				if (job)
				{
					tcsetattr(0, TCSANOW, &term_child);
					kill(job, SIGCONT);
					if (tcsetpgrp(0, job) == -1)
						ft_putendl("ERROR in seting the controling terminal to the child process");
					waitpid(job, &status, WUNTRACED);
					if (!WIFSTOPPED(status))
						job = 0;
					if (tcsetpgrp(0, getpgrp()) == -1)
						ft_putendl("ERROR in reset the controling terminal to the parent process");
					tcsetattr(0, TCSANOW, &term);
				}
			}
			else if (ft_strequ(line, "bg"))
			{
				if (job)
				{
					kill(job, SIGCONT);

				}
			}
			else
				ft_excute(line);
		}
	}
}