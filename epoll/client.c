#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>

int main(){
  struct sockaddr_in server;
  // структура для сокета
  strcpy((char*)&server.sin_addr.s_addr, (char*)host->h_addr); // адрес сервера
  server.sin_family = AF_INET;
  server.sin_port = htons(2015);
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  // создаем сокет
  int sock;
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    printf("Error create!\n");
    return -1;
  }
  // соединяемся с сервером
  if(connect(sock, (struct sockaddr*)&server, sizeof(server)) == -1){
    printf("Error connect!\n");
  }
  int i = 0;
  while(i < 10000000000) i++; // задержка
  // отправка сообщения
  send(sock, "Hello", 6, 0);
  char buf[255];
  if(recv(sock, buf, sizeof(buf), 0) < 0){
    printf("Error recv");
    return -1;
  }
  printf("Сервер: %s\n",buf);
  close(sock);
  return 0;
}
