#include "threads.h"

void* thread_serialOp(void* nullIp)
{
   nodePcIf_t data;
   int len;
   while(1)
   {
      sem_wait(&serialSem);
      // Semaphore is released. There is data available in queue 
      // Take mutex and acces data
      pthread_mutex_lock(&serialQMutex);

      /************************************/
      /**Process Serial Data queue **/
      /************************************/

      data.type = NODE_ICMP;
      /* Read the queue and fill src, dest addr and ack no*/

      
      pthread_mutex_unlock(&serialQMutex);
      if(len)
      {
         write(serialCommFd, (char*)&data, len);
         write(serialCommFd, "\n", 1);
      }
   }
}
