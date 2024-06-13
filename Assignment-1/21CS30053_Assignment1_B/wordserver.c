#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
  
#define MAXLINE 1024 
  
int main() { 
    int sockfd; 
    struct sockaddr_in servaddr, cliaddr; 
    int n; 
    socklen_t len;
    char buffer[MAXLINE]; 
      
    // Create socket file descriptor 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    servaddr.sin_family    = AF_INET; //IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(8181); 
  //  printf("s_addr: %u", servaddr.sin_addr.s_addr);


    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    printf("\nServer Running....\n");
//-------------------------------------------------------------------------
 
    len = sizeof(cliaddr);
// receive the file name
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
			( struct sockaddr *) &cliaddr, &len);
// buffer points to the text received  
    buffer[n] = '\0'; 
    printf("filename received: %s\n", buffer); 

        FILE *file;
    char filename[100];
    strcpy(filename, buffer);

// Open the file in read mode
    file = fopen(filename, "r");

    // Check if the file is opened successfully
    if (file == NULL) {
        printf("NOTFOUND %s\n", filename);
        
        char str1[] = "NOTFOUND";
        char str2[20];
        strcpy(str2, filename);
        strcat(str1,str2);
        char *hello = str1;
        sendto(sockfd, (const char *)hello, strlen(hello), 0, 
			(const struct sockaddr *) &cliaddr, sizeof(cliaddr));
             return 1;
    }
    else{
        sendto(sockfd, "HELLO", strlen("HELLO"), 0, 
			(const struct sockaddr *) &cliaddr, sizeof(cliaddr));
    }
    int c = 1;
    char line[100];

//    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
// 			( struct sockaddr *) &cliaddr, &len);
   n = 1;
     while (fgets(line, sizeof(line), file) != NULL && n != 0) {
        
        // printf("%d. %s",c,line);
        // c++;
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
			( struct sockaddr *) &cliaddr, &len);
            
        sendto(sockfd, line, strlen(line), 0, 
                    (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
        printf("server: Sent %s\n",line);    
        }
     
     fclose(file);
    close(sockfd);
    return 0; 
} 
