#include <stdio.h>
//#if _EPITKL_CFG_ARCH__x86 == _EPITKL_CFG_ARCH
//#include <errno.h>
//#include <termios.h>
//#include <unistd.h>
//#include <fcntl.h>
//#endif
#include "tinysh.h"

//#if _EPITKL_CFG_ARCH__x86 == _EPITKL_CFG_ARCH
/*--- stuff for single character input entry --------------------------------*/
//static struct termios saved_term_state;
/* remove standard input buffering to simulate minimal input capability
 * like read from the uart in an embedded environment
 */
//int set_cbreak (int fd)
//{
//	struct termios temp_term_state;
//	/* get current terminal state */
//	if (tcgetattr (fd, &saved_term_state) < 0)
//	{
//		perror ("could not get terminal state : ");
//		return (errno);
//	}
//	temp_term_state = saved_term_state;
//	temp_term_state.c_lflag &= ~(ECHO | ICANON);
//	temp_term_state.c_cc [VMIN] = 1;    /* return after 1 byte min */
//	temp_term_state.c_cc [VTIME] = 0;   /* wait forever for data */
//	/* set terminal state to cbreak */
//	if (tcsetattr (fd, TCSAFLUSH, &temp_term_state) < 0)
//	{
//		perror ("could not set terminal state : ");
//		return (errno);
//	}
//	return (0);
//}
//
//static int unset_cbreak(int fd)
//{
//	if (tcsetattr (fd, TCSAFLUSH, &saved_term_state) < 0)
//	{
//		perror ("could not set terminal state : ");
//		return (errno);
//	}
//	return 0;
//}
//#endif
//putchar_handler_t putcharHandler = NULL;
//
//void tinysh_set_putchar(putchar_handler_t handler)
//{
//  putcharHandler = handler;
//}

//void tinysh_char_out(unsigned char c)
//{
//  if (NULL != putcharHandler)
//  {
//    putcharHandler(c);
//  }
//}

void tinysh_init()
{
//	set_cbreak(0);
}

void tinysh_finish()
{
//	unset_cbreak(0);
}

