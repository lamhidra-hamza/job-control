/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msg_sig.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlamhidr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/16 19:25:51 by hlamhidr          #+#    #+#             */
/*   Updated: 2019/11/16 19:25:59 by hlamhidr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

char	*ft_getsigstr1_12(int sig)
{
	static char *str[13];
​
    if (!str[0])
	{
		ft_bzero(str, 13);
		str[0] = "+done";
		str[1] = "+hangup";
		str[2] = "-Interrupt";
		str[3] = "+Quit";
		str[4] = "+Illegal instruction";
		str[5] = "+trace trap";
		str[6] = "+abort";
		str[7] = "+EMT trap";
		str[8] = "+floating point exception";
		str[9] = "+killed";
		str[10] = "+bus error";
		str[11] = "+segmentation fault";
		str[12] = "+Bad system call";
	}
	if (sig >= 0 && sig <= 12)
		return (str[sig]);
	return (NULL);
}
​
char	*ft_getsigstr13_31(int sig)
{
	static char *str[20];
​
	if (!str[0])
	{
		str[1] = "-broken pipe";
		str[2] = "+Alarm clock";
		str[3] = "+terminated";
		str[5] = "+suspended (signal)";
		str[6] = "+suspended";
		str[9] = "+suspended (tty input)";
		str[10] = "+suspended (tty output)";
		str[12] = "+Cputime limit exceeded";
		str[13] = "+Filesize limit exceeded";
		str[14] = "+Virtual timer expired";
		str[15] = "+Profiling timer expired";
		str[18] = "+User defined signal 1";
		str[19] = "+User defined signal 2";
	}
	if (sig >= 13 && sig <= 31)
		return (str[sig - 12]);
	return (NULL);
}
​
char	*ft_strsignal(int sig)
{
	char	*str;
​
	if (sig <= 12)
		str = ft_getsigstr1_12(sig);
	else
		str = ft_getsigstr13_31(sig);
	if (str)
		return (str + 1);
	return (NULL);
}
​
char	ft_print_termsig(int status, char *name)
{
	int		sig;
	char	*msg;
​
	sig = WTERMSIG(status);
	msg = ft_strsignal(sig);
	if (msg && msg[-1] == '+')
		ft_printf("42sh : %s : %d \t %s\n", msg, sig, name);
	else
		return (0);
	return (1);
}

