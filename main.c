#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	"ftrace.h"

static pid_t   	get_args(char **av)
{
  if (av[1] && !strcmp(av[1], "-p"))
    {
      if (av[2] && !av[3])
	return (atoi(av[2]));
      if (!av[2])
	  fprintf(stderr, "%s: %s\n", av[0], EOPTP);
      return (ERROR);
    }
  return (NOOPT);
}

int		main(int ac, char **av)
{
  pid_t		pid;

  if (ac < 2 || (pid = get_args(av)) == ERROR)
    {
      fprintf(stderr, "usage: %s %s\n", av[0], EUSAGE);
      return (EXIT_FAILURE);
    }
  else if (pid == NOOPT)
    {
      if (!process_follow_by_name(ac, av))
	return (EXIT_FAILURE);
    }
  else if (!process_follow_by_pid(pid))
    return (EXIT_FAILURE);
  return (EXIT_SUCCESS);
}
