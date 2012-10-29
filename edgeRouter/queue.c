#include "queue.h"
#include <stdio.h>
#include <string.h>

sockData_t sockDataQ[MAX_SOCKET_DATAQ_ENTRIES];
serialData_t serialDataQ[MAX_SOCKET_DATAQ_ENTRIES];
IPv6EthMap_t ipAddrHashQ[MAX_HASH_ENTRIES];

unsigned char sockDataFront, sockDataRear;
unsigned char serialDataFront, serialDataRear;
unsigned char ipAddrHashFront, ipAddrHashRear;

int insert(queueId_t id, void *ptr)
{
   switch(id)
   {
      case SOCK_DATA_QUEUE:
         if(((sockDataRear + 1) % MAX_SOCKET_DATAQ_ENTRIES) ==
               sockDataFront)
         {
            return -1;
         }
         else
         {
            sockDataRear = (sockDataRear + 1) % MAX_SOCKET_DATAQ_ENTRIES;
            memcpy(&sockDataQ[sockDataRear], ptr, 
                  sizeof(sockData_t));
         }
         break;

      case SERIAL_DATA_QUEUE:
         if(((serialDataRear + 1) % MAX_NODE_DATAQ_ENTRIES) ==
               serialDataFront)
         {
            return -1;
         }
         else
         {
            serialDataRear = (serialDataRear + 1) % 
                                             MAX_NODE_DATAQ_ENTRIES; 
            memcpy(&serialDataQ[serialDataRear], ptr, 
                  sizeof(serialData_t));
         }
         break;

      case IP_ADDR_HASH_QUEUE:
         if(((ipAddrHashRear + 1) % MAX_HASH_ENTRIES) ==
               ipAddrHashFront)
         {
            return -1;
         }
         else
         {
            ipAddrHashRear = (ipAddrHashRear + 1) %
                                             MAX_HASH_ENTRIES;
            memcpy(&ipAddrHashQ[ipAddrHashRear], ptr,
                  sizeof(IPv6EthMap_t));
         }
         break;

      default:
         return -1;
   }
   return 0;
}

void* delete(queueId_t id, void* ptr)
{
   switch(id)
   {
      case SOCK_DATA_QUEUE:
         if(sockDataFront == sockDataRear)
         {
            return NULL;
         }
         else
         {
            sockDataFront = 
                  (sockDataFront + 1) % MAX_SOCKET_DATAQ_ENTRIES;
            memcpy(ptr, (void *)&sockDataQ[sockDataFront],
                     sizeof(sockData_t));
            return ptr;
         }
      break;

      case SERIAL_DATA_QUEUE:
         if(serialDataFront == serialDataRear)
         {
            return NULL;
         }
         else
         {
            serialDataFront = 
                  (serialDataFront + 1) %MAX_NODE_DATAQ_ENTRIES;
            memcpy(ptr, (void *)&serialDataQ[serialDataFront],
                           sizeof(serialData_t));
            return ptr;
         }
      break;

      case IP_ADDR_HASH_QUEUE:
         if(ipAddrHashFront == ipAddrHashRear)
         {
            return NULL;
         }
         else
         {
            ipAddrHashFront = 
                  (ipAddrHashFront + 1) % MAX_HASH_ENTRIES;
            memcpy(ptr, (void *)&ipAddrHashQ[ipAddrHashFront],
                           sizeof(IPv6EthMap_t));
            return ptr;
         }
      break;

      default:
            return NULL;
   }
}

/* 0 indicates entry not found, else number is offset from front */
int search(queueId_t id, void *ptr)
{
   int idx;
   switch(id)
   {
      case SOCK_DATA_QUEUE:
         if(sockDataRear == sockDataFront)
         {
            return 0;
         }
         else
         {
            for(idx= sockDataFront+1; idx != sockDataRear; idx = (idx + 1)%MAX_SOCKET_DATAQ_ENTRIES)
            {
               if(memcmp(ptr, sockDataQ[idx]
            }
         }
      break;

      case SERIAL_DATA_QUEUE:
      break;

      case IP_ADDR_HASH_QUEUE:
      break;

      default:
      return 0; 
   }
   return 0;
}

#if 0
/* Returns queue_id - success -1 - failure */
int create_queue(int no_of_entries, int entry_size) 
{
   static int queue_id = 0;
   queue_head[queue_id++] = malloc(no_of_entries*entry_size);
}

/* Returns 0 - success 1 - failure*/
int queue_add(char queue_id, void* entry)
{
}

/* Returns entry - success NULL - failure*/
void* queue_remove(char queue_id)
{
}

#endif
