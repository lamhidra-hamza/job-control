/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   job_control.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlamhidr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/24 19:32:46 by hlamhidr          #+#    #+#             */
/*   Updated: 2019/09/24 19:32:48 by hlamhidr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void	ft_continue(void)
{
	t_job *process;

	process = jobs->content;
	if (process->pgid == -1)
		process = jobs->next->content;
	if (process->status == STOPED)
		kill(process->pgid, SIGCONT);
}

void	ft_catch_sigchild(int sig)
{
	int pid;
	int status;
	t_job *job;
	t_list *proc;
	t_process *process;
	t_list *head;
	t_list *pr;

	head = jobs;
	pr = NULL;
	sig = 0;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		while (0 && jobs)
		{
			ft_putendl("ok");
			job = jobs->content;
			proc = job->proc;
			while (proc)
			{
				process = proc->content;
				if (pid == process->pid)
				{
					if (WIFSTOPPED(status))
						process->status = STOPED;
					else if (WIFEXITED(status))
						process->status = EXITED;
					else if (WIFSIGNALED(status))
					{
						sig = WTERMSIG(status);
						process->status = TERMINATED;
					}
							/*if (WTERMSIG(status))
							printf("[1]+  Terminated: 15\n");
						else if (job->background)
							printf("\n[%d] Done \n", 1);
						if (pr == NULL)
						{
							pr = jobs;
							jobs = jobs->next;
							free(pr->content);
							free(pr);
						}
						else
						{
							pr->next = jobs->next;
							free(jobs->content);
							free(jobs);
						}*/
					process->exit_status = status;
					break ;
				}
			}
			pr = jobs;
			jobs = jobs->next;
		}
	}
	jobs = head;
	ft_collect_job_status();
}

void	ft_foreground(void)
{
	t_job *process;
	int status;

	process = jobs->content;
	if (process->pgid == -1)
		process = jobs->next->content;
	ft_putnbr(process->pgid);
	ft_putchar('\n');
	if (process->status == STOPED || process->status == RUN)
	{
		tcsetattr(0, TCSANOW, &process->term_child);
		kill(process->pgid, SIGCONT);
		if (tcsetpgrp(0, process->pgid) == -1)
			ft_putendl_fd("ERROR in seting the controling terminal to the child process", 2);
		process->status = RUN;
		waitpid(process->pgid, &status, WUNTRACED);
		if (WIFSTOPPED(status))
			process->status = STOPED;
		if (tcsetpgrp(0, getpgrp()) == -1)
			ft_putendl_fd("ERROR in reset the controling terminal to the parent process", 2);
	}
	
}

void		ft_add_job(t_job *job)
{
	(!jobs) ? (jobs = ft_lstnew(NULL, sizeof(t_job))) :
	(ft_lstadd(&jobs, ft_lstnew(NULL, sizeof(t_job))));
	jobs->content = job;
}