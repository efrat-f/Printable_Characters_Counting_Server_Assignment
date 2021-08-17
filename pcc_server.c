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
#include <stdbool.h>
#include <signal.h>

#define MAX 95

unsigned int pcc_total[MAX];
_Bool degel_1;
_Bool degel_2;

void signal_handler(int sig){
	signal(SIGINT, SIG_IGN);
	if(degel_1){
		degel_2 = 1;
		return;
	}else{
		for (int i = 0; i<MAX ; i++ ){
			printf("char ’%c’ : %u times\n",i+32, pcc_total[i]);
		}
		exit(0);
	}
}

int main(int argc, char *argv[]){
	if(argc != 2){
			perror("Error");
			exit(1);
	}
	for (int i = 0; i<MAX ; i++ ){
		pcc_total[i] = 0;
	}
	degel_1 = 0;
	degel_2 = 0;
	struct sockaddr_in sin = {
	  	.sin_family = AF_INET,
	  	.sin_addr.s_addr = htonl(INADDR_ANY),
	  	.sin_port = htons(atoi(argv[1]))
	};
	struct sockaddr_in client;
	socklen_t addr_size = sizeof(struct sockaddr_in );
	int s = socket( AF_INET, SOCK_STREAM, 0 );
	if( 0 != bind( s, (struct sockaddr*) &sin, addr_size ) )
	{
		perror("Error");
		exit(1);
	}
	
	if( 0 != listen( s, 10 ) )
	{
		perror("Error");
		exit(1);
	}
	unsigned int len;
 	signal(SIGINT, signal_handler);
 	unsigned read_byte = 0;
 	unsigned sum_read_byte = 0; 
	unsigned sum_printable_byte = 0;
	int flag = 1;  
    if (-1 == setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {  
        perror("setsockopt fail");  
    }
 	while (1) { 
    	int new_s = accept( s, (struct sockaddr*) &client, &addr_size);
	    if( new_s < 0 )
	    {
	 		perror("Error");
			exit(1);
	    }
	    degel_1 = 1;
	    if(read(new_s, &len, sizeof(len))<0){
			perror("Error");
			exit(1);
		}
		char* buff = (char*)malloc(sizeof(char)*ntohl(len));
	    while(sum_read_byte < ntohl(len)){
		    if((read_byte = read(new_s, buff+sum_read_byte, ntohl(len)- sum_read_byte ))<0){
				perror("Error");
				exit(1);
			}sum_read_byte += read_byte;
			for(int i = 0; i<read_byte ;i++){
				if(buff[i]>31 && buff[i]< 127){
					pcc_total[buff[i]-32] +=1;
					sum_printable_byte++;
				}
			}
		}
		sum_printable_byte = htonl(sum_printable_byte);
		if(write(new_s, &sum_printable_byte, sizeof(sum_printable_byte))<0){
			perror("Error: ");
			exit(1);
		}
	    close(new_s);
	    if(degel_2){
	     	for (int i = 0; i<MAX ; i++ ){
				printf("char ’%c’ : %u times\n",i+32, pcc_total[i]);
			}
	    	return 0;
	    }
	    degel_1 = 0;
	    sum_read_byte = 0;
	    sum_printable_byte = 0;
    }
  
}
