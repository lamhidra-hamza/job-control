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

int			ft_lst_len(t_list *tmp)
{
	int i;

	i = 0;
	while (tmp)
	{
		i++;
		tmp = tmp->next;
	}
	return (i);
}

void		ft_update_index(void)
{
	t_list *tmp;
	t_job	*job;
	int		len;

	tmp = jobs;
	len = ft_lst_len(tmp);
	while (tmp)
	{
		job = tmp->content;
		job->index = len--;
		tmp = tmp->next;
	}
}

void	ft_continue(void)
{
	t_list *tmp;
	t_job *job;

	if (!jobs)
	{
		ft_putendl_fd("42sh: bg: current: no such job", 2);
		return ;
	}
	tmp = jobs;
	while (tmp)
	{
		job = tmp->content;
		if (job->status == STOPED)
		{
			ft_putchar('[');
			ft_putnbr(job->index);
			ft_putchar(']');
			(job->p) ? ft_putchar(job->p) : ft_putchar(' ');
			ft_putchar(' ');
			ft_putstr(job->cmd);
			ft_putstr(" &\n");
			job->background = 1;
			job->status = RUN;
			killpg(job->pgid, SIGCONT);
			break ;
		}
		tmp = tmp->next;
	}
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
		if (pid == process->pid)
		{
			process->status = EXITED;
			if (WIFSTOPPED(status))
				process->status = STOPED;
			if (WIFSIGNALED(status))
				job->sig_term = WTERMSIG(status);
			process->exit_status = status;
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
			break ;
		if (pid == 0 && current)
			continue;
		else if (pid == 0)
			break ;
		tmp = jobs;
		if (current)
		{
			ft_updatestatus(current, status, pid);
			if (ft_terminated(current) || ft_stoped(current))
			{
				ft_collect_job_status();
				return ;
			}
		}
		while (tmp)
		{
			job = tmp->content;
			ft_updatestatus(job, status, pid);
			tmp = tmp->next;
		}
	}
	ft_collect_job_status();
}

void	ft_catch_sigchild(int sig)
{

	sig = 0;
	ft_wait(NULL);
}

void	ft_jobs_built(void)
{
	t_list *tmp;
	t_job	*job;

	tmp = jobs;
	while (tmp)
	{
		job = tmp->content;
		ft_putchar('[');
		ft_putnbr(job->index);
		ft_putchar(']');
		(job->p != 0) ? ft_putchar(job->p) : ft_putchar(' ');
		ft_putstr("  ");
		ft_printstatus(job->status);
		ft_putstr("\t\t\t");
		ft_putstr(job->cmd);
		ft_putchar(' ');
		(job->background) ? ft_putchar('&') : 0;
		ft_putchar('\n');
		tmp = tmp->next;
	}
}

void	ft_remove_node(t_list *tmp, t_list *pr)
{
	if (pr == NULL)
	{
		tmp = tmp->next;
		free(jobs->content);
		free(jobs);
		jobs = tmp;
	}
	else
	{
		pr->next = tmp->next;
		free(tmp->content);
		free(tmp);
		tmp = pr;
	}
}

void	ft_foreground(void)
{
	t_job *job;
	t_list *tmp;
	t_list *pr;

	if (!jobs)
	{
		ft_putendl_fd("42sh: fg: current: no such job", 2);
		return ;
	}
	job = jobs->content;
	tmp = jobs;
	pr = NULL;
	while (tmp)
	{
		if (job->status == STOPED)
		{
			if (tcsetpgrp(0, job->pgid) == -1)
				ft_putendl_fd("ERROR in seting the controling terminal to the child process", 2);
			signal(SIGCHLD, SIG_DFL);
			killpg(job->pgid, SIGCONT);
			job->status = RUN;
			g_sign = 1;
			ft_wait(job);
			g_sign = 0;
			(job->sig_term != 0) ? ft_print_termsig_fore(job->sig_term, job->cmd) : 0;
			if (job->status == EXITED)
				ft_remove_node(tmp, pr);
			if (tcsetpgrp(0, getpgrp()) == -1)
				ft_putendl_fd("ERROR in reset the controling terminal to the parent process", 2);
			signal(SIGCHLD, ft_catch_sigchild);
		}
		pr = tmp;
		tmp = tmp ? tmp = tmp->next : tmp;
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


