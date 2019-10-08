#include	<stdio.h>
#include	<stdbool.h>
#include	<stdlib.h>
#include	<string.h>
#include	"ftrace.h"

char		*get_signal_name(int signal)
{
  int		i;

  i = 0;
  while (signals_tab[i].value != END && signals_tab[i].value != signal)
    ++i;
  if (signals_tab[i].value == signal)
    return (signals_tab[i].name);
  return (NULL);
}
