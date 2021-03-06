/*

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   +-----------------------------------+
   |                                   | 
   |  @author: sam@bonnekamp.net       |
   |                                   |
   |  @version: 1.2                    |
   |                                   |  
   +-----------------------------------+
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "../twitchlib.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <resolv.h>
#include <malloc.h>
#include <errno.h>

int main(){
  
  twitch_connection* twlibc = twlibc_init(1);

  
  printf("pogey socket is: %d\n", twlibc->socket);
  
  char buffer[1024];
  
  if(twlibc_setupauth(twlibc, "oauth:n0l34ks", "botbkamp", buffer, 1024)==-1){
    perror("FATAL: Couldn't authenticate with twitch servers");
  }
  
  SSL_read(twlibc->ssl, buffer, 1024);
  
  printf("%s", buffer);

  bzero(buffer, 1024);
  if(twlibc_joinchannel(twlibc, "#bkamp_", buffer, 1024)==-1){
    perror("FATAL: Couldn't join server");
  }
  printf("%s", buffer);
  
  bzero(buffer, 1024);
  if(twlibc_msgchannel(twlibc, "#bkamp_", "This bot is being tested")==-1){
    perror("FATAL: Couldn't send message");
  }
  printf("%s", buffer);

  char returnString[1024];

  if(SSL_read(twlibc->ssl, returnString, sizeof(returnString))==-1){
    perror("FATAL: couldn't read from twitch socket");
  }

  printf("%s", returnString);

  char* senderName = twlibc_parseSender(returnString);
  printf("parsed name is: %s\n", senderName);
  free(senderName);

  if(twlibc_whisper(twlibc, "Bkamp_", "test", "#bkamp_") == -1){
    perror("FATAL: couldn't whisper");
  }
  
  bzero(buffer, 1024);
  if(twlibc_leavechannel(twlibc, "#bkamp_", buffer, 1024)==-1){
    perror("FATAL: Couldn't leave server");
  }
  printf("%s", buffer);
  
}
