#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>

int main(int argc, char **argv){
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(2015);
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  // создаем сокет
  int sock;
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("Ошибка create()");
    return -1;
  }
  // соединяемся с сервером
  if(connect(sock, (struct sockaddr*)&server, sizeof(server)) == -1){
    perror("Ошибка connect()");
    return -1;
  }

  char buf[255] = "Hello From Client #";
  sprintf(buf,"%s%s",buf,argv[1]);
  while(1){
    send(sock, buf, strlen(buf), 0);
    sleep(atoi(argv[1]));
  }
  close(sock);
  return 0;
}
