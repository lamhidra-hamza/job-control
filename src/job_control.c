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
	t_list *tmp;

	sig = 0;
	ft_putstr("\nSIG CHILD HEEEEEERRE\n");
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		ft_putstr("CHILD PROCESS REPORTED pid == ");
		ft_putnbr(pid);
		ft_putchar('\n');
		tmp = jobs;
		while (tmp)
		{
			job = jobs->content;
			proc = job->proc;
			while (proc)
			{
				process = proc->content;
				ft_putstr("STATUS OF THE PROCESS BEFOR pid == ");
				ft_putnbr(process->pid);
				ft_putchar(' ');
				ft_printstatus(process->status);
				ft_putchar('\n');
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
					process->exit_status = status;
					ft_putstr("STATUS OF THE PROCESS AFTER pid == ");
					ft_putnbr(pid);
					ft_putchar(' ');
					ft_printstatus(process->status);
					ft_putchar('\n');
					break ;
				}
				proc = proc->next;
			}
			tmp = tmp->next;
		}
	}
	ft_collect_job_status();
}

void	ft_foreground(void)
{
	t_job *job;
	t_list *proc;
	t_process *process;
	int status;
	int stop;

	job = jobs->content;
	ft_putnbr(job->pgid);
	ft_putchar('\n');
	stop = 0;
	if (job->status == STOPED)
	{
		tcsetattr(0, TCSANOW, &job->term_child);
		if (tcsetpgrp(0, job->pgid) == -1)
			ft_putendl_fd("ERROR in seting the controling terminal to the child process", 2);
		proc = job->proc;
		while (proc)
		{
			process = proc->content;
			ft_putjoblst(job->pgid, process->pid, process->status);
			if (stop)
			{
				kill(process->pid, SIGTSTP);
				process->status = STOPED;
				proc = proc->next;
				continue ;
			}
			kill(process->pid, SIGCONT);
			process->status = RUN;
			if (proc->next == NULL)
				waitpid(process->pid, &status, WUNTRACED);
			if (WIFSTOPPED(status))
			{
				stop = 1;
				tcgetattr(0, &job->term_child);
				proc = job->proc;
				process->status = STOPED;
				continue ;
			}
			if (tcsetpgrp(0, getpgrp()) == -1)
				ft_putendl_fd("ERROR in reset the controling terminal to the parent process", 2);
			proc = proc->next;
		}
	}
	
}

void		ft_add_job(t_job *job)
{
	(!jobs) ? (jobs = ft_lstnew(NULL, sizeof(t_job))) :
	(ft_lstadd(&jobs, ft_lstnew(NULL, sizeof(t_job))));
	jobs->content = job;
}