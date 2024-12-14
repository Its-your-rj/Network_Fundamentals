// gcc receiver.c -o receiver; ./receiver; rm receiver; clear

#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#define PORT 8080 
#define SA struct sockaddr 
struct frame { int seq_num; char info[1024]; };

void toNetworkLayer(FILE* f, struct frame* frame){
    fprintf(f,"%s", frame->info);
}

// Function designed for chat between client and server. 
void func(int connfd, FILE* f) 
{
    struct frame frame;
    float r;
    int frame_expected = 0;
    
	while (1) {
		read(connfd, &frame, sizeof(frame)); 
		printf("From client: %d,%s", frame.seq_num, frame.info);

        if ((strncmp(frame.info, "exit", 4)) == 0) {
            send(connfd, &frame.seq_num, sizeof(frame.seq_num), 0);
		} else{
            r = (float) rand()/RAND_MAX;
            printf("%f\n",r);
            if (r >= 0.4) send(connfd, &frame.seq_num, sizeof(frame.seq_num), 0);
        }

        if (frame.seq_num == frame_expected){
            toNetworkLayer(f,&frame);
            frame_expected = 1-frame_expected;
        }

        if ((strncmp(frame.info, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}
}

// Driver function 
int main() { 
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} else printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	}
	else printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} else printf("Server listening..\n"); 
	len = sizeof(cli); 

	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("server accept failed...\n"); 
		exit(0); 
	} 
	else printf("server accept the client...\n\n"); 

    FILE* f = fopen("toNetworkLayer.txt","w");
	func(connfd,f);

	close(sockfd); 
}
