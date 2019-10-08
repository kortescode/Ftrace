#include	<elf.h>
#include	<string.h>
#include	<unistd.h>
#include	<stdarg.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<fcntl.h>
#include	<sys/stat.h>
#include	<sys/types.h>
#include	<sys/ptrace.h>
#include	"ftrace.h"
#include	"libelf.h"

void		*xmalloc(size_t size)
{
  void		*ptr;

  if (!(ptr = malloc(size)))
    {
      fprintf(stderr, "%s\n", EMALLOC);
      exit(EXIT_FAILURE);
    }
  return (ptr);
}

char		*xstrdup(const char *str)
{
  char		*ret;

  if (!(ret = strdup(str)))
    {
      fprintf(stderr, "%s\n", ESTRDUP);
      exit(EXIT_FAILURE);
    }
  return (ret);
}

ssize_t		xreadlink(const char *path, char *buf, size_t bufsiz)
{
  ssize_t       ret;

  if ((ret = readlink(path, buf, bufsiz)) == ERROR)
    {
      fprintf(stderr, "%s\n", EREADLINK);
      exit(EXIT_FAILURE);
    }
  return (ret);
}

long		xptrace(enum __ptrace_request request, pid_t pid,
			void *addr, void *data)
{
  long		ret;

  if ((ret = ptrace(request, pid, addr, data)) == ERROR)
    {
      fprintf(stderr, "%s\n", EPTRACE);
      exit(EXIT_FAILURE);
    }
  return (ret);
}

int		xprintf(const char *format, ...)
{
  int		ret;
  va_list	va;

  va_start(va, format);
  if ((ret = vprintf(format, va)) == ERROR)
    {
      va_end(va);
      fprintf(stderr, "%s\n", EPRINTF);
      exit(EXIT_FAILURE);
    }
  va_end(va);
  return (ret);
}

int		xsprintf(char *str, const char *format, ...)
{
  int		ret;
  va_list	va;

  va_start(va, format);
  if ((ret = vsprintf(str, format, va)) == ERROR)
    {
      va_end(va);
      fprintf(stderr, "%s\n", ESPRINTF);
      exit(EXIT_FAILURE);
    }
  va_end(va);
  return (ret);
}

int			xopen(const char *pathname, int flags, mode_t mode)
{
  int			fd;

  if ((fd = open(pathname, flags, mode)) == ERROR)
    {
      fprintf(stderr, "%s\n", EOPEN);
      exit(EXIT_FAILURE);
    }
  return (fd);
}

ssize_t			xwrite(int fd, const void *buf, size_t count)
{
  int			ret;

  if ((ret = write(fd, buf, count)) == ERROR)
    {
      fprintf(stderr, "%s\n", EWRITE);
      exit(EXIT_FAILURE);
    }
  return (ret);
}

unsigned int		xelf_version(unsigned int version)
{
  unsigned int		ret;

  if ((ret = elf_version(version)) == EV_NONE)
    {
      fprintf(stderr, "%s\n", EELFVERSION);
      exit(EXIT_FAILURE);
    }
  return (ret);
}

Elf			*xelf_begin(int fildes, Elf_Cmd cmd, Elf *ref)
{
  Elf			*ret;

  if (!(ret = elf_begin(fildes, cmd, ref)))
    {
      fprintf(stderr, "%s\n", EELFBEGIN);
      exit(EXIT_FAILURE);
    }
  return (ret);
}

Elf_Kind		xelf_kind(Elf *elf)
{
  Elf_Kind		ret;

  if ((ret = elf_kind(elf)) != ELF_K_ELF)
    {
      fprintf(stderr, "%s\n", EELFKIND);
      exit(EXIT_FAILURE);
    }
  return (ret);
}
