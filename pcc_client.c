#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <string.h>

int main(int argc, char **argv){
	if(argc != 4){
		perror("Error");
		exit(1);
	}
	int s = socket(AF_INET, SOCK_STREAM, 0); 
	if(s < 0){
		perror("Error");
		exit(1);
	}
	struct sockaddr_in sin = { 
		.sin_family = AF_INET, 
		.sin_port = htons(atoi(argv[2])), 
	}; 
	inet_aton(argv[1], (struct in_addr*)&(sin.sin_addr.s_addr));
	if(connect(s, (struct sockaddr*) &sin, sizeof(sin))<0){
		perror("Error");
		exit(1);
	}
	int sum_printable_byte;
	FILE *m_file = fopen( argv[3], "r" );
	if(m_file == NULL){
		 perror("Error");
		exit(1);
	}
	fseek(m_file, 0, SEEK_END); // move to end of file
	unsigned int len_1 = ftell(m_file);
	unsigned int len = htonl(len_1);
	fseek(m_file, 0, SEEK_SET); // move to start of file
	if(write(s, &len, sizeof(len))<0){
		perror("Error");
		exit(1);
	}
	char* buff=(char*)malloc(sizeof(char)*(len_1));
	int sum_write_byte = 0;
	int write_byte = 0;
	if(fread(buff, 1, len_1, m_file)<len_1){
		perror("Error: fread");
		exit(1);
	}
 	while( sum_write_byte < len_1 ){
	  	write_byte = write(s, buff + sum_write_byte, len_1 - sum_write_byte);
	    sum_write_byte  += write_byte;
	}
	 if(read(s, &sum_printable_byte, sizeof(sum_printable_byte))<=0){
		perror("Error");
		exit(1);
	}printf("# of printable characters: %u\n", ntohl(sum_printable_byte));
	fclose(m_file);
	return 0;
}
