// gcc sender.c -o sender; ./sender; rm sender; clear
#include <pthread.h>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define PORT 8080
#define SA struct sockaddr


struct frame {  int seq_num; char info[1024]; };
struct parameters { struct frame* frame; int sockfd; };

void start_timer(struct parameters* p){
    float r;
    sleep(2);
    while(1) {
        r = (float) rand()/RAND_MAX;
        printf("%f\n",r);
        if (r >= 0.7) send(p->sockfd, p->frame, sizeof(p->frame), 0);
        sleep(2);
    }
}

void fromNetworkLayer(FILE* f, struct frame* frame){
    bzero(frame->info, sizeof(frame->info));
    int n = 0; char x;
    while (1){
        x = fgetc(f);
        frame->info[n++] = x;
        if (x == '\n') break;
    }
}


void func(int sockfd, FILE* f)
{
    pthread_t timeout;
    struct frame frame;
    struct parameters parameters;

    int num;
    float r;
    frame.seq_num = 0;
    printf("Packet reading...\n");
    fromNetworkLayer(f, &frame);

	while (1) {
        printf("packet %d sending...\n",frame.seq_num);
        r = (float) rand()/RAND_MAX;
        printf("%f\n",r);
        if (r >= 0.4) send(sockfd, &frame, sizeof(frame), 0);

        parameters.sockfd = sockfd; parameters.frame = &frame;
        pthread_create(&timeout, NULL,(void*) start_timer, &parameters);

        read(sockfd,&num,sizeof(num));
        pthread_cancel(timeout);

        if (num == frame.seq_num){ // frame arival
            if ((strncmp(frame.info, "exit", 4)) == 0) {
                printf("Client Exit...\n");
                break;
            }
            frame.seq_num = 1-frame.seq_num;
            fromNetworkLayer(f,&frame);
        } else{
            printf("Incorrect ack arrival.\n");
        }
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
		!= 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
    FILE* f = fopen("fromNetworkLayer.txt","r");
	func(sockfd,f);

	// close the socket
	close(sockfd);
}

