 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define MAX_BUF 50

int main(){
    // --------------------------------------------------------------
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t clilen;
    char buf[MAX_BUF];
    int i, fd1, fd2;

    //create a socket
    if( (sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0 ){
        perror("[-]socket creation failed\n");
        exit(0);
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(10000);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
   // bind the socket
    if( (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))) < 0){
        perror("[-]bind failed\n");
        exit(0);
    }

    listen(sockfd,5);
    
// Till here------socket creation-------socket binding---------------
    while(1){
        printf("\nserver runnig..............\n");
        clilen = sizeof(client_addr);
        if((newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &clilen )) < 0){
            perror("[-]not accepted\n");
            exit(0);
        }
        int pid = fork();
    if(pid == 0){

        int k;
        recv(newsockfd, buf, MAX_BUF, 0);
        k = atoi(buf);
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("\nClient IP address: %s\n", clientIP);
        int n = 1;   
        char fname[30];
        sprintf(fname,"%s.%d.txt",clientIP,client_addr.sin_port);
        fd1 = open(fname, O_WRONLY | O_CREAT | O_TRUNC,0666); 
        // 0666 read write permissions
            
        for(i = 0; i < 100; i++)buf[i] = '\0';
            while(n){
                for(i = 0; i < 100; i++)buf[i] = '\0';
                n = recv(newsockfd,buf,MAX_BUF,0);// n contains no of bytes received
                //printf("n = %d, received: %s %d\n\n",n,buf, strlen(buf));
                int flag = 0;

                for(int i = 0; i < n; i++){
                    if(buf[i] == '$'){
                       flag = 1; break;
                    }
                    else{
                        write(fd1, &buf[i], 1);
                    }
                }
                if(flag)break;
            }
            printf("\nData written in Temporary file %s\n",fname);
            close(fd1);

        int bytes_read = 1;
        // another file X.enc
        char efname[30];
        sprintf(efname,"%s.enc", fname);

        fd2 = open(efname, O_WRONLY | O_CREAT | O_TRUNC,0666); 
        // 0666 read write permissions
        
        for(int i = 0; i < MAX_BUF; i++)buf[i] = '\0';
         //open the file fname before reading
          fd1 = open(fname, O_RDONLY);

//----------------- start of encryption-----------------------------
        while(bytes_read){
            bytes_read = read(fd1, buf, MAX_BUF);
            if(bytes_read < 0){
                perror("error reading file\n");
                exit(0);
            }
           
           for(int i = 0; i < bytes_read; i++){
                if(buf[i] == '$'){
                        break;
                }
                else if(buf[i] >= 'A' && buf[i] <= 'Z'){
                        buf[i] = 'A' + (buf[i]-'A' + k)%26;
                        write(fd2, &buf[i], 1);
                }
                else if(buf[i] >= 'a' && buf[i] <= 'z'){
                        buf[i] = 'a' + (buf[i] - 'a' + k)%26;
                        write(fd2, &buf[i], 1);
                }
                else{
                         write(fd2, &buf[i], 1);
                }
            }
        }
        close(fd1);
        printf("\nData encrypted successfully in file %s\n",efname);
//----------------- END of encryption---------------------------------------------

// start sending content of encrypted file --------------------------------
     for(int i = 0; i < MAX_BUF; i++)buf[i] = '\0';
     close(fd2);
     fd2 = open(efname,O_RDONLY);
     bytes_read = 1;

     while(bytes_read){

        bytes_read = read(fd2, buf, MAX_BUF);
        if(bytes_read < 0){
                perror("error reading file\n");
                exit(0);
            }
        send(newsockfd, buf, bytes_read,0);

     }
     // delimiter $
     send(newsockfd,"$",1,0);
     printf("\nEND OF FILE RECEIVED\n");
     printf("\nEncrypted data sent succesfully to client : %s\n",clientIP);
     close(fd2);
// end of sending content to client----------------------------------------
        close(newsockfd);
        exit(0);
    }
//--------------------child process ENDS-----------------------------
        close(newsockfd);
        
    }
return 0;
}
    

