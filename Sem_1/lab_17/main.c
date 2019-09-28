#include <stdio.h> 
#include <termios.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <fcntl.h> 
#include <string.h> 
#include <ctype.h> 
#include <stdlib.h> 

#define ERROR -1 
#define STRING_LENGTH 40
#define TUNE "\7"

typedef int(*strHandler)(int terminal, char *string, size_t *lengthString, char c);

int setNewAttr(int terminal, struct termios *newOption, struct termios *oldOption) {
	if (tcgetattr(terminal, oldOption) == ERROR) {
		perror("tcgetattr error");
		return -1;
	}

	*newOption = *oldOption;
	newOption->c_lflag &= ~(ICANON | ECHO | ISIG);
	newOption->c_cc[VMIN] = 1;
	newOption->c_cc[VTIME] = 0;

	if (tcsetattr(terminal, TCSANOW, newOption) == ERROR) {
		perror("tcsetattr error");
		return -1;
	}

	return 0;
}

int setOldAttr(int terminal, struct termios *oldOption) {
	if (tcsetattr(terminal, TCSANOW, oldOption) == ERROR) {
		perror("tcsetattr error");
		return -1;
	}
	return 0;
}

int erase(int terminal, char *string, int *lengthString, char c) {
	if (*lengthString <= 0) {
		return 0;
	}

	if(write(terminal, "\b \b", 3) == ERROR) {
		perror("write error");
		return -1;
	}

	--*lengthString;

	return 0;
}

int eraseWord(int terminal, char *string, int *lengthString, char c) {
	while (isspace(string[(*lengthString) - 1]) && *lengthString > 0) {
		if(erase(terminal, string, lengthString, c) != 0) {
			return -1;
		}
	}

	while (!isspace(string[(*lengthString) - 1]) && *lengthString > 0) {
		if(erase(terminal, string, lengthString, c) != 0) {
			return -1;
		}
	}

	return 0;
}

int eraseString(int terminal, char *string, int *lengthString, char c) {
	while (*lengthString > 0) {
		if (erase(terminal, string, lengthString, c) != 0) {
			return -1;
		}
	}

	return 0;
}

int newLine(int terminal) {
	//*lengthString = 0;
	write(terminal, "\n", strlen("\n"));
	// if (write(terminal, "\n", strlen("\n")) == ERROR) {
	// 	perror("write error");
	// 	return -1;
	// }

	return 0;
}

int tune(int terminal, char *string, int *lengthString, char c) {
	if (write(terminal, TUNE, strlen(TUNE)) == ERROR) {
		perror("write error");
		return -1;
	}

	return 0;
}

int writeWithNewLine(int terminal, int lengthString, char *currentString, char c) {
	int valueIndex = lengthString;
	while (' ' != currentString[valueIndex] && 0 != valueIndex) {
		--valueIndex;
	}
	write(terminal, "\b \b", 3);
	// for (int i = *lengthString; i != valueIndex; --i) {
	// 	if (write(terminal, "\b \b", 3) == ERROR) {
	// 		perror("write error");
	// 		return -1;
	// 	}
	// }

	newLine(terminal);
	//if(newLine(terminal) == -1){
	// 	return -1;
	// }

	while (valueIndex != STRING_LENGTH) {
		++valueIndex;

		if (write(terminal, &currentString[valueIndex], sizeof(char)) == ERROR) {
			perror("write error");
			return -1;
		}

		currentString[++lengthString] = currentString[valueIndex];
	}

	return 0;
}

int printChar(int terminal, char *string, int lengthString, char c) {
	string[lengthString] = c;
	if (STRING_LENGTH == lengthString) {    
		writeWithNewLine(terminal, lengthString, string, c);
		return 0;
	}

	++lengthString;
	if (write(terminal, &c, sizeof(char)) == ERROR) {
		perror("write error");
		return -1;
	}

	return 0;
}

strHandler getHandler(char funcName, struct termios *newOption) {
	if(CWERASE == funcName) {
		return eraseWord;
	}
	if('\n' == funcName) {
		return newLine;
	}
	if (newOption->c_cc[VKILL] == funcName) {
		return eraseString;
	}
	if (newOption->c_cc[VERASE] == funcName) {
		return erase;
	}
	if (!isprint(funcName)) {
		return tune;
	}
	
	return printChar;
}

int editor(int terminal, struct termios *newOption) {
	char buffer;
	int code;
	int lengthString = 0;
	char currentString[STRING_LENGTH] = { 0 };

	while((code = read(terminal, &buffer, 1)) > 0) {
		if(CEOT == buffer && lengthString == 0) {
			break;
		}

		strHandler handler = getHandler(buffer, newOption);

		if(handler(terminal, currentString, &lengthString, buffer) != 0) {
			return -1;
		}
	}

	if(code == ERROR) {
		perror("read error");
	}

	return 0;
}

int main() {
	int terminal = open("/dev/tty", O_RDWR);
	
	if (terminal == ERROR) {
		perror("open error");
		return -1;
	}

	if(!isatty(terminal)) {
		perror("isatty error");
		close(terminal);
		return -1;
	}

	struct termios oldOption;
	struct termios newOption;

	if(setNewAttr(terminal, &newOption, &oldOption) == -1) {
		close(terminal);
		return -1;
	}

	editor(terminal, &newOption);

	if(setOldAttr(terminal, &oldOption) == -1) {
		close(terminal);
		return -1;
	}

	close(terminal);

	return 0;
}
