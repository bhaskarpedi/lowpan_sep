
/* MAX cannot exceed  255 */
#define MAX_SOCKET_DATAQ_ENTRIES 100
#define MAX_NODE_DATAQ_ENTRIES 100
#define MAX_HASH_ENTRIES 10

#define IP6_ADDR_LEN 16
#define NODE_EUID_LEN 8
#define ETH_ADDR_LEN 6

typedef enum queueID
{
   SOCK_DATA_QUEUE,
   SERIAL_DATA_QUEUE,
   IP_ADDR_HASH_QUEUE
}queueId_t;

/* Queue 1 - socket data queue*/
typedef struct sockData{
   unsigned char src[IP6_ADDR_LEN];
   unsigned char dest[IP6_ADDR_LEN];
   unsigned short ackNo;
}
sockData_t;

/* Queue 2 - serial data queue*/
typedef struct serialData{
}serialData_t;

typedef struct IPv6EthMap{
   unsigned char ipaddr[IP6_ADDR_LEN];
   union{
   unsigned char ethaddr[ETH_ADDR_LEN];
   unsigned char euid[NODE_EUID_LEN];
   }llAddr;
}IPv6EthMap_t;

extern sockData_t sockDataQ[MAX_SOCKET_DATAQ_ENTRIES];
extern serialData_t serialDataQ[MAX_SOCKET_DATAQ_ENTRIES];
extern IPv6EthMap_t ipAddrHashQ[MAX_HASH_ENTRIES];

extern int insert(queueId_t id, void *ptr);
extern void * delete(queueId_t id, void *ptr);
extern int search(queueId_t id, void *ptr);




#if 0
/* 
   Queue head to which all queues are attached 
   10 is the max number of queues
*/
extern void *queue_head[10];

/* Returns 0 - success 1 - failure */
extern int create_queue(int no_of_entries, int entry_size); 

/* Returns 0 - success 1 - failure*/
extern int queue_add(char queue_id, void* entry);

/* Returns entry - success NULL - failure*/
extern void* queue_remove(char queue_id);
#endif
