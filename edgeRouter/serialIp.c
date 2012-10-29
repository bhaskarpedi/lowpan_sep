#include "threads.h"
#include <string.h>

void* thread_serialIp(void* nullIp)
{
   int nbytes;
   char *bufptr;
   char buffer[1024];
   nodePcIf_t *ifData;
   static sockData_t icmpReply;   // For now at least

   while(1)
   {
      memset(&icmpReply, 0, sizeof(sockData_t));
      /* read characters into our string buffer until we get a CR or NL */
      bufptr = buffer;
      while ((nbytes = read(serialCommFd, bufptr,\
          buffer + sizeof(buffer) - bufptr - 1)) > 0)
      {
         bufptr += nbytes;
         if (bufptr[-1] == '\n' || bufptr[-1] == '\r')
            break;
      }

      /* Received data from node, process it */
      ifData = (nodePcIf_t *)(&buffer[0]);

      pthread_mutex_lock(&sockQMutex);
      /* Process this received data and fill the socket data queue */
      switch(ifData->type)
      {
         /*TODO: Later
           case NODE_ICMP_REQ:
           break;
          */
         
         case NODE_ICMP:
         /* Post src address, dest address and ack number */
            memcpy(&icmpReply.src, &ifData->srcNodeAddr, IP6_ADDR_LEN);
            memcpy(&icmpReply.dest, &ifData->destNodeAddr, IP6_ADDR_LEN);
            icmpReply.ackNo = ifData->uart_data.ack_no;
            insert(SOCK_DATA_QUEUE, (void *)&icmpReply);
         break;
         
         case NODE_UDP:
         break;
         
         case NODE_OTHER:
         break;
         
         default:
         ;
      }
      pthread_mutex_unlock(&sockQMutex); 
      sem_post(&sockSem);
   }
}
