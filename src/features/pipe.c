/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pipe.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: onouaman <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/24 23:40:16 by onouaman          #+#    #+#             */
/*   Updated: 2019/07/24 23:40:17 by onouaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include "read_line.h"
/*
**	ft_close_pipes : close all pipes created  :
*/

void			ft_close_pipes(t_pipes *st_pipes)
{
	while (st_pipes != NULL)
	{
		if (st_pipes->fds[0] != 0)
			close(st_pipes->fds[0]);
		if (st_pipes->fds[1] != 0)
			close(st_pipes->fds[1]);
		st_pipes = st_pipes->next;
	}
}

/*
**	ft_create_pipes : create all pipes needed :
*/

void			ft_create_pipes(t_pipes *st_pipes)
{
	t_pipes *st_head;
	int		fds[2];

	st_head = st_pipes;
	while (st_pipes->next != NULL)
	{
		if (pipe(fds) == -1)
			ft_err_exit("Error in Pipe Function \n");
		if (st_pipes == st_head)
			st_pipes->fds[0] = fds[0];
		st_pipes->fds[1] = fds[1];
		if (st_pipes->next != NULL)
			st_pipes->next->fds[0] = fds[0];
		st_pipes = st_pipes->next;
		if (st_pipes->next == NULL)
			st_pipes->fds[1] = fds[1];
	}
}

/*
**	ft_apply_pipe_h : helper function (norme) :
*/

static void		ft_apply_pipe_h(t_pipes *st_pipes, t_pipes *st_head, int i)
{
	if (dup2(st_pipes->fds[i], i) == -1)
		ft_putendl_fd("Error in dub STD_", 2);
	ft_close_pipes(st_head);
	ft_cmd_fork(0, st_pipes);
	exit(0);
}

/*
**	ft_apply_pipe : PIPE :
*/

void	ft_lstadd_last(t_list **alst, t_list *new, t_process *proc)
{
	t_list *tmp;

	tmp = *alst;
	while (*alst != NULL)
	{
		*alst = (*alst)->next;
	}
	*alst = new;
	(*alst)->content = proc;
	*alst = tmp;
}

void			ft_fill_process(int pid, t_job *job)
{
	t_process *process;
	t_list *tmp;

	process = ft_memalloc(sizeof(t_process));
	process->pid = pid;
	process->status = RUN;
	process->exit_status = 0;
	tmp = job->proc;
	if (!job->proc)
	{
		job->proc = ft_lstnew(NULL, sizeof(t_process));
		job->proc->content = process;
	}
	else
	{
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = ft_lstnew(NULL, sizeof(t_process));
		tmp->next->content = process;
	}
}

void			ft_pipe_job_management(t_job *job, t_pipes *st_pipes, int *status, int add)
{
	t_process *p;
	t_list *proc;
	int		stop;

	p = NULL;
	proc = job->proc;
	stop = 0;
	if (!st_pipes->bl_jobctr)
	{
		if (tcsetpgrp(0, job->pgid) == -1)
			ft_putendl("ERROR in seting the controling terminal to the child process");
		g_sign = 1;
		ft_wait(job);
		g_sign = 0;
	}
	if (tcsetpgrp(0, getpid()) == -1)
		ft_putendl("ERROR in reset the controling terminal to the parent process");
	while (proc)
	{
		p = proc->content;
		*status = p->exit_status;
		proc = proc->next;
	}
	(job->status == STOPED || add) ? ft_add_job(job) : 0;
	//ft_collect_job_status();
}

int				ft_apply_pipe(t_pipes *st_pipes)
{
	t_pipes	*st_head;
	t_job	*job;
	int		status;
	int		pid;
	int		add;
	
	status = 0;
	add = 0;
	st_head = st_pipes;
	signal(SIGCHLD, SIG_DFL);
	ft_create_pipes(st_pipes);
	job = ft_inisial_job();
	while (st_pipes != NULL)
	{
		if ((pid = fork()) == 0)
		{
			ft_signal_default();
			if (st_pipes->next != NULL)
			{
				if (st_pipes != st_head && dup2(st_pipes->fds[0], 0) == -1)
					ft_putendl_fd("Error in dub STD_IN", 2);
				ft_apply_pipe_h(st_pipes, st_head, 1);
			}
			else if (st_pipes->next == NULL)
				ft_apply_pipe_h(st_pipes, st_head, 0);
			exit(EXIT_FAILURE);
		}
		else
		{
			job->cmd = ft_cmd_value(st_pipes->st_tokens, job->cmd);
			if (st_pipes->next)
				job->cmd = ft_strjoir_rtn(job->cmd, " | ", 1);
			if (job->pgid == -1)
			{
				job->pgid = pid;
				if (st_pipes->bl_jobctr)
				{
					printf("[%d] %d\n", job->index, job->pgid);
					add = 1;
					job->background = 1;
				}
			}
			setpgid(job->pgid, pid);
			job->status = RUN;
			ft_fill_process(pid, job);
		}
		st_pipes = st_pipes->next;
	}
	ft_close_pipes(st_head);
	ft_pipe_job_management(job, st_head, &status, add);
	//ft_catch_sigchild(0);
	signal(SIGCHLD, ft_catch_sigchild);
	return ((status) ? 0 : 1);
}

/*
** exec pipe
*/

int				ft_pipe(t_pipes *st_pipe)
{
	int status;

	status = 0;
	if (!st_pipe)
		return (-1); /// Check this status
	/// if exist pipe
		
	if (st_pipe && st_pipe->next)
		status = ft_apply_pipe(st_pipe);
	else
		status = ft_cmd_fork(1, st_pipe);
	return (status);
}