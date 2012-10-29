#include "threads.h"

//#define EXIT_FAILURE 1
/* Serial communication file descriptor */
int serialCommFd;
int socketCommFd;

sem_t sockSem;
sem_t serialSem;
 
pthread_mutex_t serialQMutex;
pthread_mutex_t sockQMutex;

/* TODO: Error handling */
int initialize_serial_comm()
{
   int fd;
   fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
   if(-1 == fd)
   {
      printf("Failed to open serial Communication port \n");
      exit(EXIT_FAILURE);
   }
   else
      fcntl(fd, F_SETFL, 0);

   return fd;
}

/* TODO: Error handling */
int initialize_socket_comm()
{
   int sd;

   // Open raw socket descriptor.
   if ((sd = socket (PF_PACKET, SOCK_DGRAM, htons (ETH_P_ALL))) < 0) {
      perror ("socket() failed ");
      exit (EXIT_FAILURE);
   }
   return sd;
}


int main()
{
   pthread_t thr1, thr2, thr3, thr4;
#if 0
   /* pthread_attr_t attr1, attr2, attr3, attr4; */
   pthread_attr_t attr;
   /* sched_param param1, param2, param3, param4; */
   struct sched_param param;

   pthread_getattr_np(pthread_self(),&attr);

   pthread_attr_setschedpolicy(&attr, SCHED_RR);

   pthread_attr_getschedparam(&attr, &param);
   printf("Main Thread sched priority is %d\n", param.sched_priority);
   param.sched_priority = 50; /* Reducing the priority by 1 */
   pthread_attr_setschedparam(&attr, &param);
   pthread_create(&thr1, &attr, thread_serialIp, NULL);
   pthread_create(&thr2, &attr, thread_serialOp, NULL);
   pthread_create(&thr3, &attr, thread_socketIp, NULL);
   pthread_create(&thr4, &attr, thread_socketOp, NULL);
   /* Commented above attr part as it did not work as expected */
#endif
   /* 0 - don't share with other processes
      1 - initial value of sem */
   sem_init(&sockSem, 0, 0);
   sem_init(&serialSem, 0, 0);
   pthread_mutex_init(&sockQMutex, NULL);
   pthread_mutex_init(&serialQMutex, NULL);

   serialCommFd = initialize_serial_comm();
   socketCommFd = initialize_socket_comm();
 
   pthread_create(&thr1, NULL, thread_serialIp, NULL); // need to increase priority
   pthread_create(&thr2, NULL, thread_serialOp, NULL);
   pthread_create(&thr3, NULL, thread_socketIp, NULL);
   pthread_create(&thr4, NULL, thread_socketOp, NULL); // need to increase priority

   while(1)
   {
      sleep(1000);
   }
   return 0;
}

//Extras:
   // The main thread priority is already zero. No more changes needed.
