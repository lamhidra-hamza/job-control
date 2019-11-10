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
	while (tmp != NULL)
		tmp = tmp->next;
	tmp = new;
	if (proc)
		tmp->content = proc;
}

void			ft_fill_process(int pid, t_job *job)
{
	t_process *process;

	process = ft_memalloc(sizeof(t_process));
	if (!job->proc)
	{
		job->proc = ft_lstnew(NULL, sizeof(t_process));
		job->proc->content = process;
	}
	else
		ft_lstadd_last(&job->proc, ft_lstnew(NULL, sizeof(t_process)), process);
	process->pid = pid;
	process->status = RUN;
	process->exit_status = 0;
}

void			ft_pipe_job_management(t_job *job, t_pipes *st_pipes, int *status)
{
	t_process *p;

	p = NULL;
	while (job->proc != NULL)
	{
		p = job->proc->content;
		if (!st_pipes->bl_jobctr)
		{
			if (tcsetpgrp(0, job->pgid) == -1)
				ft_putendl("ERROR in seting the controling terminal to the child process");
			g_sign = 1;
			waitpid(p->pid, &p->exit_status, WUNTRACED);
			g_sign = 0;
			if (WIFSTOPPED(p->exit_status))
			{
				job->status = STOPED;
				tcgetattr(0, &job->term_child);
			}
		}
		if (tcsetpgrp(0, getpid()) == -1)
			ft_putendl("ERROR in reset the controling terminal to the parent process");
		*status = p->exit_status;
		job->proc = job->proc->next;
	}
}

int				ft_apply_pipe(t_pipes *st_pipes)
{
	t_pipes	*st_head;
	t_job	*job;
	int		status;
	int		pid;
	
	status = 0;
	st_head = st_pipes;
	ft_create_pipes(st_pipes);
	job = ft_inisial_job();
	signal(SIGCHLD, SIG_DFL);
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
			if (job->pgid == -1)
			{
				job->pgid = pid;
				if (st_pipes->bl_jobctr)
				{
					printf("[%d] %d\n", job->index, job->pgid);
					ft_add_job(job);
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
	ft_pipe_job_management(job, st_head, &status);
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