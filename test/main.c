#include	<stdio.h>
#include	<stdlib.h>

static void	toto(void)
{
  (void)printf("%s\n", "Je suis dans toto()");
}

static void	tutu(void)
{
  (void)printf("%s\n", "Je suis dans tutu()");
}

int		main(void)
{
  toto();
  tutu();
  exit(EXIT_SUCCESS);
}
