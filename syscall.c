#include	<stdio.h>
#include	<string.h>
#include	<stdbool.h>
#include	<stdlib.h>
#include	<sys/user.h>
#include	"ftrace.h"

char		*get_syscall_name(long opcode)
{
  int		i;

  i = 0;
  while (syscalls_tab[i].value != END && syscalls_tab[i].value != opcode)
    ++i;
  if (syscalls_tab[i].value == opcode)
    return (syscalls_tab[i].name);
  return (NULL);
}
