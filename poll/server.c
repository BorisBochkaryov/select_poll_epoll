#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <sys/time.h>

int main(){
  struct sockaddr_in server, client;
  int sock;
  // создание сокета
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    printf("Error create!\n");
    return -1;
  }
  // структура для сервера
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY; // локальный адрес
  server.sin_port = htons((u_short)2015); // порт сервера
  // связка
  if(bind(sock, (struct sockaddr*)&server, sizeof(server)) == -1){
    printf("Error bind!\n");
    return -1;
  }
  // объявляем готовность к соединению
  if(listen(sock,5)==-1){
    printf("Error listen!\n");
    return -1;
  }
  // создаем опрос дескрипторов 
  struct pollfd fds[10];
  fds[0].fd = sock;
  fds[0].events = POLLIN;
  int newsock, clnlen; // сокеты для клиентов
  int nfds = 1;
  while(1){
    int ret = poll(fds,nfds,-1);
    if(ret == -1){
      printf("Error poll()!\n");
      return -1;
    }
    printf("Что-то происходит...\n");
    if(fds[0].revents & POLLIN){ // если произошло событие на сокете сервера
      printf("Соединение с сервером!\n");
      if((newsock = accept(sock, (struct sockaddr*)&client, &clnlen)) == -1){ // подключаем нового пользователя
        printf("Error accept!\n");
        return -1;
      }
      fds[nfds].fd = newsock;
      fds[nfds].events = POLLIN;
      nfds++;
    }
    int j;
    for(j = 1; j < nfds; j++){
      if(fds[j].revents & POLLIN){ // если произошло событие на сокете клиента
        printf("Данные: ");
        int size;
        char buf[255];
        size = recv(newsock, buf, sizeof(buf), 0); // пока получаем от клиента
        send(newsock, buf, sizeof(buf), 0); // отправляем эхо
        printf("%s\n",buf);
        close(newsock); // отключаем пользователя, т.к. он нам больше не нужен
        nfds--;
      }
    }
  }
  close(sock);
  return 0;
}
