#ifndef			FTRACE_H_
# define		FTRACE_H_

# include		<elf.h>
# include		<unistd.h>
# include		<stdlib.h>
# include		<stdbool.h>
# include		<sys/user.h>
# include		<sys/ptrace.h>
# include		"libelf.h"

/**********************
***** Return type *****
**********************/
typedef	enum		e_return
  {
    FAILURE,
    SUCCESS
  }			t_return;

/***********************
***** Error values *****
***********************/
# define		ERROR		-1

/************************
***** Error strings *****
************************/
# define		EUSAGE		"[-p pid] [command [arg ...]]"
# define		EOPTP		"option requires an argument -- 'p'"
# define		ECMD		"command not found"
# define		EATTACH		"attach: ptrace(PTRACE_ATTACH, ...): No such process"

# define		EELFVERSION	"error: elf library loading failed"
# define		EELFKIND	"error: the agument is not an ELF object"

# define		EFCT		"error: can't perform "
# define		EEXECVP		EFCT "execvp()"
# define		EPRINTF		EFCT "printf()"
# define		ESTRDUP		EFCT "strdup()"
# define		ESPRINTF       	EFCT "sprintf()"
# define		EMALLOC		EFCT "malloc()"
# define		EPTRACE		EFCT "ptrace()"
# define		EREADLINK	EFCT "readlink()"
# define		EWAIT4		EFCT "wait4()"
# define		ESIGNAL		EFCT "signal()"
# define		EFORK		EFCT "fork()"
# define		EOPEN		EFCT "open()"
# define		EWRITE		EFCT "write()"
# define		EELFBEGIN	EFCT "elf_begin()"

/**************************
***** No option value *****
**************************/
# define		NOOPT		-2

/***********************************
***** Opcodes recognize macros *****
***********************************/
# define		IS_CALL_E8(x)	(((x) & 0xff) == 0xe8)
# define		IS_CALL_FF2(x)	(((x) & 0xff) == 0xff && (((x) >> 11) & 0x7) == 0x2)
# define		IS_CALL_FF3(x)	(((x) & 0xff) == 0xff && (((x) >> 11) & 0x7) == 0x3)

# define		IS_SYSCALL(x)	(((x) & 0xffff) == 0x050f)
# define		IS_SYSINT(x)	(((x) & 0xffff) == 0x80cd)
# define		IS_SYSCENTER(x)	(((x) & 0xffff) == 0x340f)

# define		IS_JMP(x)	(((x) & 0xffff) == 0x25ff)

# define		IS_RET1(x)	(((x) & 0xff) == 0xc2 || ((x) & 0xff) == 0xc3)
# define		IS_RET2(x)	(((x) & 0xff) == 0xca || ((x) & 0xff) == 0xcb)
# define		IS_RET(x)	(IS_RET1(x) || IS_RET2(x))

/***************************************
***** Mod R/M and SIB utils macros *****
***************************************/
# define		GET_MOD(x)	  (((x) >> 14) & 0x3)
# define		GET_REG(x)	  (((x) >> 11) & 0x7)
# define		GET_RM(x)	  (((x) >> 8) & 0x7)

# define		GET_SCALE(x)	  (((x) >> 22) & 0x3)
# define		GET_INDEX(x)	  (((x) >> 19) & 0x7)
# define		GET_BASE(x)	  (((x) >> 16) & 0x7)

# define		GET_DISP8(x)	  (((x) & 0xff0000) >> 16)
# define		GET_SIB_DISP8(x)  (((x) & 0xff000000) >> 24)
# define		GET_DISP32(x)	  (((x) & 0xffff0000) >> 16)
# define		GET_SIB_DISP32(x) (GET_SIB_DISP8(x))


/*********************
***** Tab struct *****
*********************/
typedef struct		s_tab
{
  int			value;
  char			*name;
}			t_tab;

/***********************
***** Syscalls tab *****
***********************/
extern t_tab		syscalls_tab[];

/**********************
***** Signals tab *****
**********************/
extern t_tab		signals_tab[];

/**********************
***** List struct *****
**********************/
typedef struct		s_list
{
  long			value;
  char			*name;
  struct s_list		*next;
}			t_list;

/*****************************
***** End value for tabs *****
*****************************/
# define		END		ERROR

/******************************
***** List of call struct *****
******************************/
typedef struct		s_call
{
  char			*name;
  char			*father;
  long			addr;
  long			rip;
  struct s_call		*next;
}			t_call;

/**************************
***** Calls type enum *****
**************************/
typedef enum		s_type
  {
    INT_FCT,
    EXT_FCT,
    SYSCALL,
    SIGNAL
  }			t_type;

/*********************
***** Dotty file *****
*********************/
# define		DCMD		"dotty"
# define		DFILE		"graph.dot"

/******************************
***** Functions prototype *****
******************************/
t_return       		process_follow_by_pid(pid_t);
t_return       		process_follow_by_name(int, char**);

t_return       		ftrace(pid_t, char**);

char			*get_signal_name(int);

char			*get_syscall_name(long);

void			init_calls_list(t_list**, char*);
char			*get_call_name(t_list**, long);
void			free_calls_list(t_list**);

long			get_reg(int, struct user_regs_struct*);
long			get_addr_from_modrm(pid_t, int, struct user_regs_struct*);
long			get_addr_from_sib(int, struct user_regs_struct*);

int			init_dotty(void);
char			*add_node_in_dotty(int, t_call**, char*, char*, long, long, t_type);
char			*get_father_in_dotty(t_call**, long);
t_return		exec_dotty(int, t_call**);

void			*xmalloc(size_t);
char			*xstrdup(const char*);
ssize_t			xreadlink(const char*, char*, size_t);
long			xptrace(enum __ptrace_request, pid_t, void*, void*);
int			xprintf(const char*, ...);
int			xsprintf(char*, const char*, ...);
int			xopen(const char*, int, mode_t);
ssize_t			xwrite(int, const void*, size_t);
unsigned int		xelf_version(unsigned int);
Elf			*xelf_begin(int, Elf_Cmd, Elf*);
Elf_Kind		xelf_kind(Elf*);

#endif			/* FTRACE_H_ */
