#include	<stdio.h>
#include	<string.h>
#include	<fcntl.h>
#include	"ftrace.h"

int		init_dotty(void)
{
  int		fd;
  char		*start;

  start = "digraph G\n{\n\t\"_start\" [color=blue, shape=box]\n";
  fd = xopen(DFILE, O_WRONLY | O_CREAT | O_TRUNC,
	     S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  xwrite(fd, start, strlen(start));
  return (fd);
}

char		*add_node_in_dotty(int fd, t_call **calls, char *name, char *father,
				   long addr, long rip, t_type type)
{
  char		buffer[256], *color;
  t_call       	*tmp, *new_call;

  new_call = xmalloc(sizeof(t_call) * 1);
  new_call->name = name;
  new_call->father = father;
  new_call->addr = addr;
  new_call->rip = rip;
  new_call->next = NULL;
  if (*calls)
    {
      tmp = *calls;
      while (tmp && tmp->next)
	tmp = tmp->next;
      tmp->next = new_call;
    }
  else
    *calls = new_call;
  color = (type == SYSCALL) ? "red" : (type == INT_FCT) ? "blue" : (type == SIGNAL) ? "black" : "green";
  xsprintf(buffer, "\t\"%s\" [color=%s, shape=box]\n", name, color);
  xwrite(fd, buffer, strlen(buffer));
  return (name);
}

char		*get_father_in_dotty(t_call **calls, long addr)
{
  t_call       	*tmp;

  if (*calls)
    {
      tmp = *calls;
      while (tmp)
	{
	  if (addr == tmp->rip)
	    return (tmp->father);
	  tmp = tmp->next;
	}
    }
  return (NULL);
}

static void	free_dotty(t_call **calls)
{
  t_call	*tmp, *del;

  tmp = *calls;
  while (tmp)
    {
      del = tmp;
      tmp = tmp->next;
      free(del);
    }
}

static void	write_links(int fd, int *list, t_call **calls)
{
  int		i, j, l, count;
  char		buffer[256];
  t_call       	*tmp, *node, *first;

  i = 0;
  l = 0;
  first = *calls;
  tmp = first;
  while (42)
    {
      j = 0;
      while (tmp)
	{
	  if (j == i)
	    {
	      j = 0;
	      count = 0;
	      node = tmp;
	      tmp = first;
	      while (tmp)
		{
		  if (tmp->name == node->name && tmp->father == node->father)
		    {
		      ++count;
		      list[l++] = j;
		      list[l] = END;
		    }
		  ++j;
		  tmp = tmp->next;
		}
	      xsprintf(buffer, "\t\"%s\" -> \"%s\" [label=%d]\n",
		       node->father, node->name, count);
	      xwrite(fd, buffer, strlen(buffer));
	      tmp = first;
	      break;
	    }
	  ++j;
	  tmp = tmp->next;
	}
      if (!tmp)
	break;
      j = 0;
      while (list[j] != END)
	{
	  if (i == list[j])
	    {
	      ++i;
	      j = 0;
	      continue;
	    }
	  ++j;
	}
    }
}

t_return	exec_dotty(int fd, t_call **calls)
{
  int		i, *list;
  char		**dotty;
  t_call       	*tmp;

  dotty = xmalloc(sizeof(char*) * 3);
  dotty[0] = DCMD;
  dotty[1] = DFILE;
  dotty[2] = NULL;
  i = 0;
  tmp = *calls;
  while (tmp)
    {
      ++i;
      tmp = tmp->next;
    }
  list = xmalloc(sizeof(int) * ++i);
  list[0] = END;
  write_links(fd, list, calls);
  xwrite(fd, "}\n", 2);
  if (execvp(dotty[0], dotty) == ERROR)
    {
      fprintf(stderr, "%s\n", EEXECVP);
      return (FAILURE);
    }
  free_dotty(calls);
  free(dotty);
  free(list);
  return (SUCCESS);
}
