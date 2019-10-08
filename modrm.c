#include	<stdio.h>
#include	"ftrace.h"

static long	mod_00(pid_t pid, int opcode, struct user_regs_struct *data)
{
  int	        rm;
  long		addr;

  rm = GET_RM(opcode);
  if (rm == 0x4)
    addr = get_addr_from_sib(opcode, data);
  else if (rm == 0x5)
    return (data->rip + (int)GET_DISP32(opcode));
  else
    addr = get_reg(rm, data);
  return (xptrace(PTRACE_PEEKTEXT, pid, (void*)addr, NULL));
}

static long	mod_01(pid_t pid, int opcode, struct user_regs_struct *data)
{
  int	        rm;
  long		addr;

  rm = GET_RM(opcode);
  if (rm == 0x4)
    addr = get_addr_from_sib(opcode, data) + (char)GET_SIB_DISP8(opcode);
  else if (rm == 0x5)
    addr = data->rbp + (char)GET_DISP8(opcode);
  else
    addr = get_reg(rm, data) + (char)GET_DISP8(opcode);
  return (xptrace(PTRACE_PEEKTEXT, pid, (void*)addr, NULL));
}

static long	mod_10(pid_t pid, int opcode, struct user_regs_struct *data)
{
  int	        rm;
  long		addr;

  rm = GET_RM(opcode);
  if (rm == 0x4)
    addr = get_addr_from_sib(opcode, data) + (int)GET_SIB_DISP32(opcode);
  else if (rm == 0x5)
    addr = data->rbp + (int)GET_DISP32(opcode);
  else
    addr = get_reg(rm, data) + (int)GET_DISP32(opcode);
  return (xptrace(PTRACE_PEEKTEXT, pid, (void*)addr, NULL));
}

static long	mod_11(__attribute__((unused))pid_t pid, int opcode, struct user_regs_struct *data)
{
  int			rm;

  rm = GET_RM(opcode);
  return (get_reg(rm, data));
}

long		get_addr_from_modrm(pid_t pid, int opcode, struct user_regs_struct *data)
{
  int		mod;

  mod = GET_MOD(opcode);
  if (mod == 0x0)
    return (mod_00(pid, opcode, data));
  else if (mod == 0x1)
    return (mod_01(pid, opcode, data));
  else if (mod == 0x2)
    return (mod_10(pid, opcode, data));
  else if (mod == 0x3)
    return (mod_11(pid, opcode, data));
  return (0);
}
