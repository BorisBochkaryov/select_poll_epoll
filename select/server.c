#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>

int main(){
  struct sockaddr_in server, client;
  int sock;
  // создание сокета
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("Ошибка create()");
    return -1;
  }
  // структура для сервера
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY; // локальный адрес
  server.sin_port = htons((u_short)2015); // порт сервера
  // связка
  if(bind(sock, (struct sockaddr*)&server, sizeof(server)) == -1){
    perror("Ошибка bind()");
    return -1;
  }
  printf("Порт сервера: %d\n",ntohs(server.sin_port));

  if(listen(sock,5)==-1){
    perror("Ошибка listen()");
    return -1;
  }

  int numClient = 0, i;
  int clients[1000];
  while(1){
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    int max = sock;
    for(i = 0; i < numClient; i++){
      FD_SET(clients[i], &fds);
      if(clients[i] > max) max = clients[i];
    }
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int rel = select(max + 1, &fds, NULL, NULL, &tv);
    if(rel == -1){
      perror("Ошибка select()");
      return -1;
    }
    if(FD_ISSET(sock, &fds)){
      int newsock, clnlen;
      if((newsock = accept(sock, (struct sockaddr*)&client, &clnlen)) == -1){
        perror("Ошибка accept()");
        return -1;
      }
      clients[numClient] = newsock;
      numClient++;
      printf("Новый клиент с порта %d!\n",ntohs(client.sin_port));

      char buf[255] = "";
      memset(buf,0,255);
      int size = recv(newsock, buf, sizeof(buf), 0);
      printf("%s\n",buf);
    } else {
      for(i = 0; i < numClient; i++){
        if(FD_ISSET(clients[i], &fds)){
          char buf[255] = "";
          memset(buf,0,255);
          int size = recv(clients[i], buf, sizeof(buf), 0);
          printf("Клиент опять прислал: %s\n",buf);
          break;
        }
      }
    }
  }
  close(sock);
  return 0;
}
