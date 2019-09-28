#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#define ERR_TERMINAL -1
#define ERR_ANSWER -1

// 				How struct termio looks like:
// struct termio {
// 	unsigned short c_iflag;		/* input mode flags */
// 	unsigned short c_oflag;		/* output mode flags */
// 	unsigned short c_cflag;		 control mode flags 
// 	unsigned short c_lflag;		/* local mode flags */
// 	unsigned char c_line;		/* line discipline */
// 	unsigned char c_cc[NCC];	/* control characters */
// };

void fillTermiosStruct(struct termios* tty){
	// ISIG - when any of the characters  INTR,  QUIT,  SUSP,  or DSUSP are received, generate the corresponding signal.
	// ICANON - enable canonical mode: 
	// 		This  enables  the  special characters  EOF,  EOL,  EOL2, ERASE, KILL, REPRINT, STATUS, and WERASE, and buffers by lines.
	
	tty->c_lflag &= ~(ISIG | ICANON);
	tty->c_cc[VMIN] = 1;
}

int main()
{
	char symbol;
	struct termios tty;
	struct termios savetty;
	
	//check stdin is it terminal or not
	if( !isatty(0) ) {
		perror("ERR: stdin != tty \n");
		return ERR_TERMINAL;
	}

	printf("Yes or no? Say [y] or [n]: ");
	fflush(stdout); // printf waits for '\n'
	tcgetattr(0, &tty); // get current tty atributes
	savetty = tty; // backup of attributes
	fillTermiosStruct(&tty);
	
	tcsetattr(0, TCSAFLUSH, &tty);
	read(0, &symbol, 1);
	//tcsetattr(0, TCSANOW, &savetty);
	//почему настройки все равно восстановятся? И как убить процесс - создать другой процесс и послать ему сигнал килл
	
	if ( symbol != 'y' && symbol != 'n'){
		printf("\n incorrect answer \n");
		return ERR_ANSWER;
	}

	else {
		printf("\n %s \n", (symbol == 'y') ? ("yes!") : ("no!") );
	}
	
	return 0;
}
