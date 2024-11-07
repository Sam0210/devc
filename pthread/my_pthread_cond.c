/*
  用于生产者与消费者模型
  必须配合mutex使用。cond用于线程的阻塞，mutex用于同步线程。
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

struct Node {
	int num;
	struct Node* next;
};

pthread_cond_t cond;   // 有几个条件限制，就创建几个条件变量（此处仅有链表下限判断）
pthread_mutex_t mutex;

struct Node* head = NULL;


void* producer(void* arg)
{
	int* pid = (int*)arg;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
		newNode->num = rand() % 1000;
		newNode->next = head;
		head = newNode;
		printf("Producer Thread %d generates number %d\n", *pid, newNode->num);
		pthread_mutex_unlock(&mutex);

		pthread_cond_signal(&cond);  // 唤醒至少一个阻塞线程

		sleep(rand() % 3);
	}
	return NULL;
}


void* consumer(void* arg)
{
	int* pid = (int*)arg;
	while(1)
	{
		pthread_mutex_lock(&mutex);

		while(head == NULL)  // if会有bug，原因在于被唤醒的线程们在wait处抢锁，抢到后直接往下执行，不会判断head是否为空！！！
			pthread_cond_wait(&cond, &mutex);  // 必须放在互斥锁之后，否则重复加锁造成死锁
	
		struct Node* node = head;
		printf("Consumer Thread %d kills number %d\n", *pid, node->num);
		head = node->next;
		free(node);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main(void)
{
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);

	int ppid[5] = {0, 1, 2, 3, 4};
	int cpid[5] = {0, 1, 2, 3, 4};
	pthread_t ptid[5], ctid[5];

	for(int i = 0; i < 5; ++i)
		pthread_create(&ptid[i], NULL, producer, &ppid[i]);
	for(int i = 0; i < 5; ++i)
		pthread_create(&ctid[i], NULL, consumer, &cpid[i]);

	for(int i = 0; i < 5; ++i)
		pthread_join(ptid[i], NULL);
	for(int i = 0; i < 5; ++i)
		pthread_join(ctid[i], NULL);

	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
	return 0;
}
