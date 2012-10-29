#include "threads.h"

void* thread_socketIp(void* nullIp)
{
   while(1)
   {
      printf("This is socket Ip thread\n");
      sleep(1);
   }
}
