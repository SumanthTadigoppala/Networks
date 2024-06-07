//Client Implementation
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
  
int main() { 
    int sockfd, err; 
    struct sockaddr_in servaddr, cliaddr; 
    int n;
    socklen_t len; 
    char hello[100];

    printf("Enter the file name: ");
    scanf("%s",hello);

 
    FILE *file;
  
    // Creating socket file descriptor 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Server information start---------------------------------------- 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(8181); 

    err = inet_aton("127.0.0.1", &servaddr.sin_addr);

    if (err == 0) {
	   printf("Error in ip-conversion\n");
	   exit(EXIT_FAILURE);
     }
    // Server information End------------------------------------------

   char buf_client[1024]; 

   // sends the file name
    sendto(sockfd, (const char *)hello, strlen(hello), 0, 
			(const struct sockaddr *) &servaddr, sizeof(servaddr)); 

    printf("Hello message sent from client\n");

    // receives messasge from server i.e "HELLO" if file name found 
    //else "NOTFOUND <filename>"
     n = recvfrom(sockfd, (char *)buf_client, 1024, 0, 
			( struct sockaddr *) &cliaddr, &len);
    buf_client[n] = '\0';

    printf("%d\n",strcmp("HELLO",buf_client));
    
    // if file ---------------------------------------------------------
    if(strcmp("HELLO",buf_client) == 0){
      printf("%s\n", buf_client);
        
      // Open a file in write mode ("w"). If the file doesn't exist, it will be newfile.
      file = fopen("newfile.txt", "w");

      if (file == NULL) {
          fprintf(stderr, "Unable to open the file.\n");
          return 1; // Exit with an error code
       }
       else{
            printf("-------------------------------------\n");
            while(strcmp("END", buf_client) != 0) {
                sendto(sockfd, "WORD1", strlen("WORD1"), 0, 
                        (const struct sockaddr *) &servaddr, sizeof(servaddr)); 

                len = sizeof(cliaddr);

                n = recvfrom(sockfd, (char *)buf_client, 1024, 0, 
                    (struct sockaddr *) &cliaddr, &len);

                if (n < 0) {
                    perror("Error receiving data from server");
                    break;
                }

                buf_client[n] = '\0';
                printf("client: Received: %s\n", buf_client);

                // Write the content to the file
                fprintf(file, "%s", buf_client);
                fflush(file);
            }
        printf("-------------------------------------\n");
    }
    }
    // present-----------------------------------------------------------
    else{
        printf("File %s Not Found\n",hello);
        exit(0);
    }

    
  printf("\nSuccessfully created and Written in newfile.txt\n\n"); 

   //fclose(file);  
    getchar();       
    close(sockfd); 
    return 0; 
} 
