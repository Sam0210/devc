/*
信号量与条件变量的区别？
	--- 信号量用于对固定数量资源的控制和简单的互斥需求（资源数=1时），不依赖条件状态;
	--- 状态变量用于在特定条件满足时唤醒等待的线程，通常需要配合互斥锁确保条件判断的原子性。

使用场景区分：
	信号量
	--- 资源访问控制：当多个线程需要访问一个有限资源（如线程池中的线程数或有限的数据库连接）时，可以使用信号量限制同时访问资源的线程数。
	条件变量
	--- 等待特定状态：当需要多个线程等待某个共享资源达到某种状态时，比如等待队列有新任务或等待某个条件满足时。
	--- 生产者消费者模型
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

// #define SINGLE

struct Node {
	int number;
	struct Node* next;
};

sem_t psem;
sem_t csem;
#ifndef SINGLE
pthread_mutex_t mutex;
#endif 
struct Node* head = NULL;


void* producer(void* arg)
{
	while(1)
	{
		sem_wait(&psem);  // 必须放在互斥锁之前，否则死锁
#ifndef SINGLE
		pthread_mutex_lock(&mutex);
#endif
		struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
		newNode->number = rand() % 1000;
		newNode->next = head;
		head = newNode;
		printf("Producer: id=%lu, number=%d\n", pthread_self(), newNode->number);
#ifndef SINGLE
		pthread_mutex_unlock(&mutex);
#endif
		sem_post(&csem);

		sleep(rand() % 3);
	}
	return NULL;
}


void* consumer(void* arg)
{
	while(1)
	{
		sem_wait(&csem);
#ifndef SINGLE
		pthread_mutex_lock(&mutex);
#endif
		struct Node* pNode = head;
		printf("Consumer: id=%lu, number=%d\n", pthread_self(), pNode->number);
		head = pNode->next;
		free(pNode);
#ifndef SINGLE
	pthread_mutex_unlock(&mutex);
#endif
		sem_post(&psem);

		sleep(rand() % 1);
	}
	return NULL;
}


int main(void)
{
#ifdef SINGLE
	sem_init(&psem, 0, 1);
#else
	sem_init(&psem, 0, 5);
#endif
	sem_init(&csem, 0, 0);
#ifndef SINGLE
	pthread_mutex_init(&mutex, NULL);
#endif

	pthread_t ptid[5], ctid[5];
	for(int i = 0; i < 5; ++i)
		pthread_create(&ptid[i], NULL, producer, NULL);
	for(int i = 0; i < 5; ++i)
		pthread_create(&ctid[i], NULL, consumer, NULL);
	
	for(int i = 0; i < 5; ++i)
		pthread_join(ptid[i], NULL);
	for(int i = 0; i < 5; ++i)
		pthread_join(ctid[i], NULL);

	sem_destroy(&psem);
	sem_destroy(&csem);
#ifndef SINGLE
	pthread_mutex_destroy(&mutex);
#endif

	return 0;
}
