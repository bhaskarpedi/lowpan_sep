#include <semaphore.h>
#include <pthread.h>

/* socket op thread waits on this */
extern sem_t sockSem;

/* Mutex to protect access to sock Data queue*/
extern pthread_mutex_t sockQMutex;

/* serial op thread waits on this */
extern sem_t serialSem;

/* Mutex to protect access to serial Data queue*/
extern pthread_mutex_t serialQMutex;
