/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   job_processing.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlamhidr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/10 00:28:15 by hlamhidr          #+#    #+#             */
/*   Updated: 2019/11/10 00:28:17 by hlamhidr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

void	ft_printstatus(int status)
{
	ft_putstr("status == ");
	if (status == STOPED)
		ft_putendl("STOPED");
	if (status == RUN)
		ft_putendl("RUN");
	if (status == TERMINATED)
		ft_putendl("TERMINATED");
	if (status == EXITED)
		ft_putendl("EXITED");
}

void	ft_putjoblst(int pgid, int pid, int status)
{
	ft_putstr("\n################\n");
	ft_putstr("pgid == ");
	ft_putnbr(pgid);
	ft_putchar('\n');

	ft_putstr("pid == ");
	ft_putnbr(pid);
	ft_putchar('\n');

	ft_printstatus(status);
}

void    ft_collect_job_status(void)
{
	t_list *tmp;
	t_list *proc;
	t_job  *job;
	t_process *process;
	int     n_proc;
	int     exited;
	int     terminated;

	tmp = jobs;
	while (tmp)
	{
		job = tmp->content;
		proc = job->proc;
		n_proc = 0;
		exited = 0;
		terminated = 0;
		ft_putstr("job ============\n");
		while (proc)
		{
			process = proc->content;
			ft_putjoblst(job->pgid, process->pid, process->status);
			if (process->status == STOPED)
			{
				job->status = STOPED;
				break ;
			}
			if (process->status == TERMINATED)
				terminated++;
			if (process->status == EXITED)
				exited++;
			n_proc++;
			proc = proc->next;
		}
		if (n_proc == exited + terminated && n_proc)
			job->status = EXITED;
		tmp = tmp->next;
	}
}

void    ft_job_processing(void)
{
	t_list *tmp;
	t_job   *job;
	t_list *pr;

	tmp = jobs;
	pr  = NULL;

	while (tmp)
	{
		job = tmp->content;
		if (job->status == STOPED)
			ft_putstr("[1]+  Stopped\n");	
		if (job->status == EXITED)
		{
			ft_putstr("[1]  Done\n");
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
		pr = tmp;
		tmp = tmp ? tmp->next : tmp;
	}
}
