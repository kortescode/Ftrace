#include			<elf.h>
#include			<string.h>
#include			<unistd.h>
#include			<stdbool.h>
#include			<stdio.h>
#include			<stdlib.h>
#include			<fcntl.h>
#include			<sys/user.h>
#include			<sys/types.h>
#include			<sys/time.h>
#include			<sys/resource.h>
#include			<sys/wait.h>
#include			<sys/ptrace.h>
#include			"ftrace.h"
#include			"libelf.h"

static char			*is_call(int fd, pid_t pid, int opcode,
					struct user_regs_struct *data,
					 t_list **calls_list, t_call **calls,
					 char *father)
{
  long				addr;

  if (IS_CALL_E8(opcode))
    {
      addr = (opcode >> 8) + data->rip + 0x5;
      if (get_call_name(calls_list, addr))
	father = add_node_in_dotty(fd, calls, get_call_name(calls_list, addr),
				   father, data->rip, data->rip + 0x5, INT_FCT);
    }
  else if (IS_CALL_FF2(opcode))
    {
      addr = get_addr_from_modrm(pid, opcode, data);
      if (get_call_name(calls_list, addr))
	father = add_node_in_dotty(fd, calls, get_call_name(calls_list, addr),
				   father, data->rip, data->rip + 0x5, INT_FCT);
    }
  else if (IS_CALL_FF3(opcode))
    {
      addr = get_addr_from_modrm(pid, opcode, data);
      if (get_call_name(calls_list, addr))
	father = add_node_in_dotty(fd, calls, get_call_name(calls_list, addr),
				   father, data->rip, data->rip + 0x5, INT_FCT);
    }
  else if (IS_SYSCALL(opcode))
    {
      if (get_syscall_name(data->rax))
	(void)add_node_in_dotty(fd, calls, get_syscall_name(data->rax),
				father, 0, 0, SYSCALL);
    }
  else if (IS_JMP(opcode))
    {
      addr = (long int)((opcode >> 16) | 0x200000) + data->rip + 0x6;
      if (get_call_name(calls_list, addr))
	father = add_node_in_dotty(fd, calls, get_call_name(calls_list, addr),
				   father, data->rip, data->rip + 0x6, EXT_FCT);
    }
  else if (IS_RET(opcode))
    {
      addr = xptrace(PTRACE_PEEKTEXT, pid, (void*)data->rsp, NULL);
      if (get_father_in_dotty(calls, addr))
	father = get_father_in_dotty(calls, addr);
    }
  return (father);
}

static bool			is_signal(int fd, pid_t pid, t_call **calls, char *father)
{
  int				signal;
  int				status;

  if (wait4(pid, &status, WUNTRACED, NULL) == ERROR)
    {
      fprintf(stderr, "%s\n", EWAIT4);
      return (false);
    }
  if (WIFEXITED(status))
    return (false);
  else if (WIFSTOPPED(status) && (signal = WSTOPSIG(status))
	   != SIGTRAP && signal != SIGSTOP)
    {
      (void)add_node_in_dotty(fd, calls, get_signal_name(signal),
			      father, 0, 0, SIGNAL);
      return (false);
    }
  else if (WIFSIGNALED(status))
    {
      (void)add_node_in_dotty(fd, calls, get_signal_name(WTERMSIG(status)),
			      father, 0, 0, SIGNAL);
      return (false);
    }
  return (true);
}

t_return       			ftrace(pid_t pid, char **argv)
{
  int				fd;
  long				opcode;
  char				*father;
  t_call			*calls;
  t_list       			*calls_list;
  struct user_regs_struct	data;

  calls = NULL;
  calls_list = NULL;
  father = "_start";
  fd = init_dotty();
  init_calls_list(&calls_list, argv[0]);
  while (is_signal(fd, pid, &calls, father))
    {
      xptrace(PTRACE_GETREGS, pid, NULL, &data);
      opcode = xptrace(PTRACE_PEEKTEXT, pid, (void*)data.rip, NULL);
      father = is_call(fd, pid, opcode, &data, &calls_list, &calls, father);
      if (ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL) == ERROR)
	break;
    }
  free_calls_list(&calls_list);
  if (!exec_dotty(fd, &calls))
    return (FAILURE);
  return (SUCCESS);
}
