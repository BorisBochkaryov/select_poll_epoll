#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/epoll.h>

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
  int epfd = epoll_create(5);
  if(epfd == -1){
    printf("Error epoll_create()\n");
    return -1;
  }
  struct epoll_event ev;
  ev.data.fd = sock;
  ev.events = EPOLLIN | EPOLLET | EPOLLHUP;
  if(epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev) == -1){ // добавляем дескриптор в структуру
    printf("Error epoll_ctl!\n");
    return -1;
  }
  struct epoll_event events[1024];
  int newsock, clnlen; // сокеты для клиентов
  while(1){
    int ready = epoll_wait(epfd, events, 1024, -1); // ожидаем событие
    if(ready == -1){
      printf("Error epoll_wait()\n");
      return -1;
    }
    int j;
    for(j = 0; j < ready; j++){
      if(events[j].data.fd == sock){ // если произошло событие на сокете сервера
        printf("Подключение к серверу!\n");
        if((newsock = accept(sock, (struct sockaddr*)&client, &clnlen)) == -1){ // подключаем нового пользователя
          printf("Error accept!\n");
          return -1;
        }
        ev.events = EPOLLIN; // событие входящей информации 
        ev.data.fd = newsock; // добавляем сокет в структуру
        if(epoll_ctl(epfd, EPOLL_CTL_ADD, newsock, &ev) == -1){ // добавляем дескриптор в структуру
          printf("Error epoll_ctl!\n");
          return -1;
        }
      } else if(events[j].data.fd == newsock){ // если событие произошло на клиентском сокете
        printf("Прием данных от клиента!\n"); 
        int size;
        char buf[255];
        size = recv(newsock, buf, sizeof(buf), 0); // пока получаем от клиента
        send(newsock, buf, sizeof(buf), 0); // отправляем эхо
        printf(">>>%s\n",buf);
  	close(newsock); // отключаем пользователя, т.к. он нам больше не нужен
      }
    }
  }
  close(sock);
  return 0;
}
