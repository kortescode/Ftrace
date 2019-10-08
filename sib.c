#include	"ftrace.h"

long		get_reg(int opt, struct user_regs_struct *data)
{
  if (opt == 0x0)
    return (data->rax);
  else if (opt == 0x1)
    return (data->rcx);
  else if (opt == 0x2)
    return (data->rdx);
  else if (opt == 0x3)
    return (data->rbx);
  else if (opt == 0x4)
    return (data->rsp);
  else if (opt == 0x5)
    return (data->rbp);
  else if (opt == 0x6)
    return (data->rsi);
  else if (opt == 0x7)
    return (data->rdi);
  return (0);
}

long		get_addr_from_sib(int opcode, struct user_regs_struct *data)
{
  int		scale;
  int		index;
  int		base;

  scale = GET_SCALE(opcode);
  index = GET_INDEX(opcode);
  base = GET_BASE(opcode);
  if (index == 0x4)
    return (get_reg(base, data));
  return (get_reg(base, data) + (get_reg(index, data) * scale));
}
