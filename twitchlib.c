/*

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   +-----------------------------------+
   |                                   | 
   |  @author: sam@bonnekamp.net       |
   |                                   |
   |  @version: 0.3.1                  |
   |                                   |  
   +-----------------------------------+
*/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX* InitCTX(void);

typedef struct twitch_connection {
  SSL_CTX* ctx;
  SSL* ssl;
  int socket;
} twitch_connection;


twitch_connection* twlibc_init(int usingSSL, struct twitch_connection* tw_conn){
  struct sockaddr_in twitchaddr;
  SSL_CTX *ctx;
  SSL *ssl;
  int twitchsock;
  int PORT;

  if(usingSSL == 1){
    //using SSL
    SSL_library_init();
    ctx = InitCTX();  
    ssl = SSL_new(ctx);
  }else {
    ctx = NULL;
    ssl = NULL;
  }
  
  twitchsock = socket(AF_INET, SOCK_STREAM, 0);
  if (twitchsock==-1)
    return -1;
  
  struct hostent* host = gethostbyname("irc.chat.twitch.tv");
  if(host == NULL)
    return -1;


  if(usingSSL == 1){
    ssl_connection = ssl;
    PORT = 6697;
  }else {
    PORT = 6667;
  }
  
  //setup address
  bzero(&twitchaddr, sizeof(twitchaddr));
  twitchaddr.sin_family = AF_INET;
  twitchaddr.sin_addr.s_addr = *(long *)host->h_addr_list[0];
  twitchaddr.sin_port = htons(PORT);

  if(connect(twitchsock, (struct sockaddr*)&twitchaddr, sizeof(twitchaddr)) != 0)
    return -1;

  if(usingSSL == 1)
    SSL_set_fd(ssl, twitchsock);
  
  if (SSL_connect(ssl) == -1)
    return -1;
  
  if(usingSSL == 0){
    //not using SSL
    tw_conn->ctx = NULL;
    tw_conn->ssl = NULL;
  }else {
    //using SSL
    tw_conn->ctx = ctx;
    tw_conn->ssl = ssl;
  }
  
  tw_conn->socket = twitchsock;
}

int twlibc_msgchannel(int sockfd, const char* channel, const char* message){
  char payload[12+strlen(channel)+strlen(message)];
  sprintf(payload, "PRIVMSG %s :%s\r\n", channel, message);
  if(usingSSL == 0){
    return write(sockfd, payload, strlen(payload));
  }else{
    return SSL_write(tw_conn->ssl, payload, strlen(payload));
  }
  return 0;
}

int twlibc_joinchannel(struct twitch_connection* tw_conn, const char* channel, char* output, int length){
  char payload[7+strlen(channel)];
  sprintf(payload, "JOIN %s\r\n", channel);
  if(tw_conn->ssl == NULL){
    //not using SSL
    if(write(sockfd, payload, strlen(payload))==-1){
      return -1;
    }    
    if(output != NULL){
      if(read(sockfd, output, length)==-1){
	return -1;
      }
    }
  }else{
    //using SSL
    if(SSL_write(tw_conn->ssl, payload, strlen(payload))==-1){
      return -1;
    }
    if(output != NULL){
      if(SSL_read(tw_conn->ssl, output, length)==-1){
	return -1;
      }
    }
  }
  return 0;
}

int twlibc_leavechannel(int sockfd, const char* channel, char* output, int length){
  char payload[7+strlen(channel)];
  sprintf(payload, "PART %s\r\n", channel);
  if(write(sockfd, payload, strlen(payload))==-1){
    return -1;
  }
  if(output != NULL){
    if(read(sockfd, output, length)==-1){
      return -1;
    }
  }
  return 0;
}


int twlibc_sendrawpacket(int sockfd, char* payload){
  int payloadlen = strlen(payload);
  /* 
  if(payload[payloadlen] != '\n' || payload[payloadlen-1] != '\r'){
    strcat(payload, "\r\n");
  }
  */
  if(usingSSL == 1){
    return SSL_write(tw_conn->ssl, payload, strlen(payload));
  }else {
    if(write(sockfd, payload, strlen(payload))==-1){
      return -1;
    }
  }
  return 1;
}

int twlibc_setupauth(struct twitch_connection* tw_conn, const char* oauth, const char* nick, char* output, int length){
  char payload[14 + strlen(oauth) + strlen(nick)];
  sprintf(payload, "PASS %s\r\nNICK %s\r\n", oauth, nick);

  if(tw_conn->ssl != NULL){
    //using SSL
    return SSL_write(tw_conn->ssl, payload, strlen(payload)); 
  }else {
    //not using SSL
    return write(sockfd, payload, strlen(payload)); 
  }
   
  if(output!=NULL){
    if(tw_conn->ssl == NULL){
      //not using SSL
      return read(sockfd, output, length);
    }else {
      return SSL_read(tw_conn->ssl, output, length);
    }
  }
  return 0;
}

char* twlibc_parseSender(char* message){
  char* name = strtok(message, "!");
  if(name==NULL)
    return NULL;
  name++;
  char* parsedName = (char *)malloc(1+strlen(name));
  if(parsedName==NULL)
    return NULL;
  strcpy(parsedName, name);
  return parsedName;
}

int twlibc_whisper(struct twitch_connection tw_conn, const char* user, const char* message, const char* channel){
  char buffer[16 + strlen(channel) + strlen(user) + strlen(message)];
  sprintf(buffer, "PRIVMSG %s :/w %s %s\r\n", channel, user, message);
  if(tw_conn->ssl != NULL){
    return SSL_write(tw_conn->ssl, buffer, strlen(buffer));
  }else {
    return write(sockfd, buffer, strlen(buffer));
  }
}



SSL_CTX* InitCTX(void){
    SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLSv1_2_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void ShowCerts(SSL* ssl){
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}
