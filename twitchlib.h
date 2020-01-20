#ifndef twitchlib
int msgchannel(int sockfd, const char* channel, const char* message);
int joinchannel(int sockfd, const char* channel, char* output, int length);
int leavechannel(int sockfd, const char* channel, char* output, int length);
int sendrawpacket(int sockfd, char* payload);
int setupauth(int sockfd, const char* oauth, const char* nick, char* output, int length);
char* parseSender(char* message);
#endif
