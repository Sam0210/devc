/*
  读锁共享：A线程加读锁对B线程加读锁/访问临界区无影响
  写锁独占：在写锁被打开之前，其他线程均不可读和写临界变量
  写锁优先级高于读锁

  相较于互斥锁，读写锁在对共享资源的读操作越多时，效率更高。
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 50

int number;
pthread_rwlock_t rwlock;

void* readNum(void* arg)
{
	while(1)
	{
		pthread_rwlock_rdlock(&rwlock);
		printf("Read thread id: %ld, num: %d\n", pthread_self(), number);
		pthread_rwlock_unlock(&rwlock);
		usleep(rand() % 100);
	}
	return NULL;
}

void* writeNum(void* arg)
{
	while(1)
	{
		pthread_rwlock_wrlock(&rwlock);
		int cur = number;
		++cur;
		number = cur;
		printf("Write thread id: %ld, num: %d\n", pthread_self(), number);
		pthread_rwlock_unlock(&rwlock);
		usleep(rand() % 100);
	}
	return NULL;
}

int main(void)
{
	pthread_rwlock_init(&rwlock, NULL);

	pthread_t rtid[5], wtid[3];
	for (int i = 0; i < 5; ++i)
		pthread_create(&rtid[i], NULL, readNum, NULL);
	for (int i = 0; i < 3; ++i)
		pthread_create(&wtid[i], NULL, writeNum, NULL);
	
	for (int i = 0; i < 5; ++i)
		pthread_join(rtid[i], NULL);
	for (int i = 0; i < 3; ++i)
		pthread_join(wtid[i], NULL);
	
	pthread_rwlock_destroy(&rwlock);
		
	return 0;
}
