#include	<stdlib.h>
#include	<signal.h>
#include	<stdio.h>
#include	<unistd.h>
#include	<sys/ptrace.h>
#include	"ftrace.h"

pid_t		gpid;

static void	ptrace_detach(__attribute__((unused))int sig)
{
  (void)xptrace(PTRACE_DETACH, gpid, NULL, NULL);
  xprintf("Process %d detached\n", gpid);
  exit(EXIT_SUCCESS);
}

static char	**get_file(pid_t pid)
{
  int		ret;
  char		path[64];
  char		*file;
  char		**argv;

  xsprintf(path, "/proc/%d/exe", pid);
  file = xmalloc(sizeof(char) * 256);
  ret = xreadlink(path, file, 255);
  file[ret] = '\0';
  argv = xmalloc(sizeof(char*) * 2);
  argv[0] = file;
  argv[1] = NULL;
  return (argv);
}

t_return       	process_follow_by_pid(pid_t pid)
{
  char		**argv;

  gpid = pid;
  argv = get_file(pid);
  if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == ERROR)
    {
      fprintf(stderr, "%s\n", EATTACH);
      return (FAILURE);
    }
  if (signal(SIGINT, ptrace_detach) == SIG_ERR)
    {
      fprintf(stderr, "%s\n", ESIGNAL);
      return (FAILURE);
    }
  xprintf("Process %d attached - interrupt to quit\n", pid);
  if (!ftrace(pid, argv))
    return (FAILURE);
  xprintf("\nProcess %d detached\n", pid);
  return (SUCCESS);
}

static char	**get_argv(int ac, char **av)
{
  int		i;
  char		**argv;

  i = 0;
  argv = xmalloc(sizeof(char*) * ac);
  while (av[i + 1])
    {
      argv[i] = av[i + 1];
      ++i;
    }
  argv[i] = NULL;
  return (argv);
}

static t_return	lauch_prgmm(char **av, pid_t pid, char **argv)
{
  xptrace(PTRACE_TRACEME, pid, NULL, NULL);
  if (execvp(argv[0], argv) == ERROR)
    {
      fprintf(stderr, "%s: %s: %s\n", av[0], argv[0], ECMD);
      return (FAILURE);
    }
  return (SUCCESS);
}

t_return       	process_follow_by_name(int ac, char **av)
{
  pid_t		pid;
  char		**argv;

  argv = get_argv(ac, av);
  if ((pid = fork()) == ERROR)
    {
      fprintf(stderr, "%s\n", EFORK);
      return (FAILURE);
    }
  else if (pid == 0)
    {
      if (!lauch_prgmm(av, pid, argv))
	return (FAILURE);
    }
  else if (!ftrace(pid, argv))
    return (FAILURE);
  free(argv);
  return (SUCCESS);
}
