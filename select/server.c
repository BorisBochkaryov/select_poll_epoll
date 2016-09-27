#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/select.h>
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
  int newsock, clnlen; // сокеты для клиентов
  
  while(1){
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int ret = select(sock + 1, &fds, NULL, NULL, &tv);
    if(ret == -1){
      printf("Error select()!\n");
      return -1;
    }
    if(FD_ISSET(sock, &fds)){
      printf("Соединение с сервером!\n");
      if((newsock = accept(sock, (struct sockaddr*)&client, &clnlen)) == -1){ // подключаем нового пользователя
        printf("Error accept!\n");
        return -1;
      }
      printf("Данные: ");
      int size;
      char buf[255];
      size = recv(newsock, buf, sizeof(buf), 0); // пока получаем от клиента
      send(newsock, buf, sizeof(buf), 0); // отправляем эхо
      printf("%s\n",buf);
      close(newsock); // отключаем пользователя, т.к. он нам больше не нужен
    }
  }
  close(sock);
  return 0;
}
