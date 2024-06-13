 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define MAX_BUF 100

int main(){
    while(1){
    int sockfd;
    struct sockaddr_in serv_addr;
    char buf[MAX_BUF];
    int i;
    char fname[20];
    int fileDescriptor, k;


    //create socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("[-]socket creation failed\n");
        exit(0);
    }
    // NO NEED TO BIND
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(10000);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);

    printf("\nfile name: ");
    scanf("%s",fname);
    fileDescriptor = open(fname, O_RDONLY);
    while (fileDescriptor == -1) {
        printf("enter valid file name: ");
       scanf("%s",fname);
       fileDescriptor = open(fname, O_RDONLY);
        
    }
    printf("enter k: ");
    scanf("%d",&k);
    char intStr[20];
    sprintf(intStr, "%d", k); // convert integer into string

    //connect with the server
     if((connect(sockfd,(struct sockaddr*) &serv_addr, sizeof(serv_addr))) < 0){
        perror("[-]connection failed\n");
        exit(0);
     }
    //sleep(10);

    for(i=0; i < MAX_BUF; i++) buf[i] = '\0';
    // Send the string representation of the integer to the server
    send(sockfd, intStr, strlen(intStr) + 1, 0);
    size_t bytes_read = 1;
    sleep(1);
    while(bytes_read){
        for (i = 0; i < MAX_BUF; i++) buf[i] = '\0';
//
        bytes_read = read(fileDescriptor,buf, sizeof(buf));
        
        // printf("read: %s, bytes_read = %d\n\n",buf, bytes_read);
        // here in send replaced strlen(buf) + 1 with  bytes_read then working
        
        send(sockfd, buf, bytes_read, 0);
    }
    
    send(sockfd,"$",1,0);
    printf("\nData sent!! closing file %s\n",fname);
    close(fileDescriptor);
    
    int n = 1;
    int fd;
    char filename[20];
    sprintf(filename,"%s.enc",fname);
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,0666);
    while(n){
        n = recv(sockfd, buf, MAX_BUF,0);
        int flag = 0;
        for(int i = 0; i < n; i++){
                    if(buf[i] == '$'){
                        break;
                        flag = 1;
                    }
                   
                    else{
                         write(fd, &buf[i], 1);
                    }
                }
        if(flag) break;
        }
       
    
    close(fd);
    printf("\nFile is ENCRYPTED!!\n Original File: %s\n\n Encrypted File: %s\n",fname,filename);
		
	close(sockfd);
    }

	return 0;

}