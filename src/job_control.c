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
#include "read_line.h"

void	ft_continue(void)
{
	t_job *process;

	process = jobs->content;
	if (process->status == STOPED)
		killpg(process->pgid, SIGCONT);
}

char	ft_stoped(t_job *job)
{
	t_process *proc;
	t_list		*lst;

	lst = job->proc;
	while (lst)
	{
		proc = lst->content;
		if (proc->status == STOPED)
		{
			job->status = STOPED;
			return (1);
		}
		lst = lst->next;
	}
	return (0);
}

char	ft_terminated(t_job *job)
{
	t_process *proc;
	t_list *lst;

	lst = job->proc;
	while (lst)
	{
		proc = lst->content;
		if (proc->status != EXITED)
			return (0);
		lst = lst->next;
	}
	job->status = EXITED;
	return (1);
}

void	ft_updatestatus(t_job *job, int status, int pid)
{
	t_list *proc;
	t_process *process;

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
			process->status = EXITED;
			if (WIFSTOPPED(status))
				process->status = STOPED;
			if (WIFSIGNALED(status))
				job->sig_term = WTERMSIG(status);
			process->exit_status = status;
			ft_putstr("STATUS OF THE PROCESS AFTER pid == ");
			ft_putnbr(pid);
			ft_putchar(' ');
			ft_printstatus(process->status);
			ft_putchar('\n');
		}
		proc = proc->next;
	}
}

void ft_wait(t_job *current)
{
	int pid;
	int status;
	t_job *job;
	t_list *tmp;

	while (1)
	{
		if ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) < 0)
			break;
		if (pid == 0 && current)
			continue;
		else if (pid == 0)
		{
			ft_putstr("breaking\n");
			break;
		}
		// ft_putstr("CHILD PROCESS REPORTED pid == ");
		// ft_putnbr(pid);
		// ft_putchar('\n');
		tmp = jobs;
		if (current)
		{
			ft_updatestatus(current, status, pid);
			if (ft_terminated(current) || ft_stoped(current))
					return ;
		}
		else
			while (tmp)
			{
				job = tmp->content;
				ft_updatestatus(job, status, pid);
				tmp = tmp->next;
			}
	}
}

void	ft_catch_sigchild(int sig)
{

	sig = 0;
	//ft_putstr("\nSIG CHILD HEEEEEERRE\n");
	ft_wait(NULL);
	ft_collect_job_status();
}

void	ft_foreground(void)
{
	t_job *job;

	job = jobs->content;
	ft_putnbr(job->pgid);
	ft_putchar('\n');
	if (job->status == STOPED)
	{
		if (tcsetpgrp(0, job->pgid) == -1)
			ft_putendl_fd("ERROR in seting the controling terminal to the child process", 2);
		killpg(job->pgid, SIGCONT);
		job->status = RUN;
		g_sign = 1;
		ft_wait(job);
		g_sign = 0;
		if (tcsetpgrp(0, getpgrp()) == -1)
			ft_putendl_fd("ERROR in reset the controling terminal to the parent process", 2);
	}
}

void		ft_update_p(void)
{
	t_list *tmp;
	t_job	*job;
	int		add;

	tmp = jobs;
	add = 0;
	while (tmp)
	{
		job = tmp->content;
		if (add == 0)
			job->p = '+';
		if (add == 1)
			job->p = '-';
		if (add > 1)
			job->p = 0;
		add++;
		tmp = tmp->next;
	}
}

void		ft_add_job(t_job *job)
{
	(!jobs) ? (jobs = ft_lstnew(NULL, sizeof(t_job))) :
	(ft_lstadd(&jobs, ft_lstnew(NULL, sizeof(t_job))));
	jobs->content = job;
	ft_update_p();
}

int			ft_job_index(void)
{
	t_job *job;

	if (jobs)
	{
		job = jobs->content;
		return (job->index + 1);
	}
	return (1);
}








