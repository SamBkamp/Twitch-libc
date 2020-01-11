#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int msgchannel(int sockfd, const char* channel, const char* message){
  char* payload = (char *)malloc(12+strlen(channel)+strlen(message));
  sprintf(payload, "PRIVMSG %s :%s\r\n", channel, message);
  if(write(sockfd, payload, strlen(payload))==-1){
    free(payload);
    return -1;
  }
  free(payload);
  return 0;
}

int joinchannel(int sockfd, const char* channel, char output){
  char* payload = (char *)malloc(7+strlen(channel));
  char buff[1024];
  sprintf(payload, "JOIN %s\r\n", channel);
  if(write(sockfd, payload, strlen(payload))==-1){
    free(payload);
    return -1;
  }
  if(output != NULL){
    if(read(twitchsock, buff, sizeof(buff))==-1){
      free(payload);
      return -1;
    }
    strncpy(output, buff, sizeof(output));
  }
  free(payload);
  return 0
}
