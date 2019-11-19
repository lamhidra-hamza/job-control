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
	if (status == STOPED)
		ft_putstr("Stopped");
	if (status == RUN)
		ft_putstr("Running");
	if (status == TERMINATED)
		ft_putstr("Terminated");
	if (status == EXITED)
		ft_putstr("Exited");
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
		while (proc)
		{
			process = proc->content;
		//	ft_putjoblst(job->pgid, process->pid, process->status);
			if (process->status == STOPED)
				job->status = STOPED;
			if (process->status == TERMINATED)
				terminated++;
			if (process->status == EXITED)
				exited++;
			n_proc++;
			proc = proc->next;
		}
		if (job->status != STOPED && n_proc == exited + terminated && n_proc)
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

	ft_update_p();
	ft_update_index();
	//ft_catch_sigchild(0);
	while (tmp)
	{
		job = tmp->content;
		// ft_printstatus(job->status);
		// ft_putchar('\n');
		if (job->status == STOPED && !job->mark_stop)
		{
			job->mark_stop = 1;
			ft_putchar('\n');
			ft_print_termsig_back(32, job->cmd, job->index, job->p);
		}
		if (job->status == EXITED)
		{
			ft_print_termsig_back(job->sig_term, job->cmd, job->index, job->p);
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
