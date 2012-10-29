#include <stdio.h>
#include <stdlib.h>
#include <string.h>           // strcpy, memset(), and memcpy()
#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket()
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_RAW, IPPROTO_ICMPV6
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <netinet/ip6.h>      // struct ip6_hdr
#include <netinet/icmp6.h>    // struct icmp6_hdr and ICMP6_ECHO_REQUEST
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>
#include <pthread.h>
#include <errno.h>            // errno, perror()
#include <unistd.h>
#include "semaphore.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "queue.h"

/*****************************************************************************/
                        /* Macro defines*/
/*****************************************************************************/

//#define NODE_ADDR_LEN 128

#define IP6_HDRLEN 40         // IPv6 header length
#define ICMP_HDRLEN 8         // ICMP header length for echo request, excludes data

#define INTERFACE_TO_USE "eth1"
#define SRC_IP_ADDR "2001::1"
#define DEST_IP_ADDR "2001::2"

/*****************************************************************************/
                        /* externs */
/*****************************************************************************/

extern int serialCommFd;
extern int socketCommFd;

extern sem_t sockSem;
extern sem_t serialSem;

extern pthread_mutex_t serialQMutex;
extern pthread_mutex_t sockQMutex;

/*****************************************************************************/
                        /* Enum defines*/
/*****************************************************************************/
typedef enum protoType{
   /* TODO: Later 
   NODE_ICMP_REQ,
   NODE_ICMP_REP,
   */
   NODE_ICMP,
   NODE_UDP,
   NODE_OTHER
}protoType_t;


/*****************************************************************************/
                        /* Structure and union defines*/
/*****************************************************************************/

typedef union uartData{
   /* This field is for ICMP */
   unsigned short ack_no;
   /* This field is for UDP, TCP, Others */
   struct varData{
      unsigned char len;
      unsigned char data[sizeof(unsigned char)];
   }varData;
}uartData_t;

typedef struct nodePcIf{
   unsigned char srcNodeAddr[IP6_ADDR_LEN];
   unsigned char destNodeAddr[IP6_ADDR_LEN];
   protoType_t type;
   uartData_t uart_data;
} nodePcIf_t;

/*****************************************************************************/
                        /* Thread function handles */
/*****************************************************************************/

/* Thread purpose:
   1. Blocked wait on serial port
   2. Fill socket data queue to be used by socketOp thread
   3. Release socket binary semaphore to enable socketOp thread
*/
extern void* thread_serialIp(void*);

/* Thread purpose:
   1. Blocked wait on node binary semaphore (released by socketIp)
   2. Tx data to the nodes from the node data queue
*/
extern void* thread_serialOp(void*);

/* Thread purpose:
   1. Blocked wait on raw socket 
   2. Process only ICMP messages intended for the nodes and discard the rest
   3. Prepare the ICMP output and save it in ICMP list (along with ACK and 
      node addr for easy transmission by socketOp when reply received from node)
   4. Fill node data queue
   5. release node binary semaphore
*/
extern void* thread_socketIp(void*);

/* Thread purpose:
   1. Blocked on socket binary semaphore
   2. Read data (ack no, node addr) from socket data queue
   3. Look up the ICMP reply from ICMP list and transmit
*/
extern void* thread_socketOp(void*);
