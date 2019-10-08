#include	<string.h>
#include	<stdio.h>
#include	<fcntl.h>
#include	"ftrace.h"
#include	"libelf.h"

static void	add_call_in_tab(t_list **calls_list, long addr, char *name)
{
  t_list       	*tmp;
  t_list       	*new_call;

  if (name && strlen(name) != 0 && addr != 0x0)
    {
      new_call = xmalloc(sizeof(t_list) * 1);
      new_call->value = addr;
      new_call->name = xstrdup(name);
      new_call->next = NULL;
      if (*calls_list)
	{
	  tmp = *calls_list;
	  while (tmp && tmp->next)
	    tmp = tmp->next;
	  tmp->next = new_call;
	}
      else
	*calls_list = new_call;
    }
}

static void    	add_call_rel(t_list **calls_list, Elf *elf,
			     Elf_Scn *scn, Elf64_Shdr *shdr)
{
  size_t       	i;
  Elf_Scn	*scn_link;
  Elf64_Shdr	*shdr_link;
  Elf64_Sym	*sym_link;
  Elf64_Rela	*rela;
  Elf_Data	*data, *data_link;

  i = 0;
  data = elf_getdata(scn, NULL);
  scn_link = elf_getscn(elf, shdr->sh_link);
  shdr_link = elf64_getshdr(scn_link);
  data_link = elf_getdata(scn_link, NULL);
  while (i < shdr_link->sh_size)
    {
      rela = (Elf64_Rela*)((long int)data->d_buf + i);
      if (ELF64_R_TYPE(rela->r_info) == R_386_JMP_SLOT)
	{
	  sym_link = (Elf64_Sym*)((long)data_link->d_buf +
		     ((ELF64_R_SYM(rela->r_info) * shdr_link->sh_entsize)));
	  add_call_in_tab(calls_list, rela->r_offset, elf_strptr(elf,
			  shdr_link->sh_link, sym_link->st_name));
	}
      i += shdr_link->sh_entsize;
    }
}

static void	add_call_symtab(t_list **calls_list, Elf *elf,
				Elf_Scn *scn, Elf64_Shdr *shdr)
{
  Elf64_Sym	*symtab;
  size_t       	i, symtab_size;

  i = 0;
  symtab = (elf_getdata(scn, NULL))->d_buf;
  symtab_size = (elf_getdata(scn, NULL))->d_size / sizeof(Elf64_Sym);
  while (i < symtab_size)
    {
      add_call_in_tab(calls_list, symtab[i].st_value, elf_strptr(elf,
		      shdr->sh_link, symtab[i].st_name));
      ++i;
    }
}

void		init_calls_list(t_list **calls_list, char *file)
{
  int		fd;
  Elf		*elf;
  Elf_Scn	*scn;
  Elf64_Shdr	*shdr;
  size_t       	shdrstrndx;

  scn = NULL;
  fd = xopen(file, O_RDONLY, 0);
  xelf_version(EV_CURRENT);
  elf = xelf_begin(fd, ELF_C_READ_MMAP, NULL);
  xelf_kind(elf);
  elf_getshdrstrndx(elf, &shdrstrndx);
  while ((scn = elf_nextscn(elf, scn)))
    {
      shdr = elf64_getshdr(scn);
      if (shdr->sh_type == SHT_RELA || shdr->sh_type == SHT_REL)
	add_call_rel(calls_list, elf, scn, shdr);
      else if (shdr->sh_type == SHT_SYMTAB)
	add_call_symtab(calls_list, elf, scn, shdr);
    }
  elf_end(elf);
  close(fd);
}

char		*get_call_name(t_list **calls_list, long addr)
{
  t_list        *tmp;

  tmp = *calls_list;
  while (tmp)
    {
      if (addr == tmp->value)
	return (tmp->name);
      tmp = tmp->next;
    }
  return (NULL);
}

void		free_calls_list(t_list **calls_list)
{
  t_list	*tmp, *del;

  tmp = *calls_list;
  while (tmp)
    {
      del = tmp;
      tmp = tmp->next;
      free(del);
    }
}
