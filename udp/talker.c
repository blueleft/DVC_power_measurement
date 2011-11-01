#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>


#define SERVERPORT "4950"    // the port users will be connecting to
#define PAYLOAD 512

int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }

    int fd = open(argv[2],O_RDONLY);
    if(fd<0){
	perror("talker:open file");
	exit(1);
    }
    int n;
    char buf[PAYLOAD];

    while((n=read(fd,buf,PAYLOAD))>0){
	if ((numbytes = sendto(sockfd, buf, n, 0,
	         p->ai_addr, p->ai_addrlen)) == -1) {
	    perror("talker: sendto");
	    exit(1);
	}
	printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
    }

    freeaddrinfo(servinfo);
    
    close(sockfd);

    return 0;
}
