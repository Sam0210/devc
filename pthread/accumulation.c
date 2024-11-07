/* 临界变量/共享变量num在没有线程同步的保护下，无法正确累加100次。
   被cpu处理完成后的最新数据并未更新到物理内存中，其余子线程只能
       拿到旧的数据往后数，白干活。*/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 50

int num;

void* funA(void* arg)
{
	for (int i = 0; i < MAX; ++i)
	{
		int tmp = num;
		++tmp;
		usleep(10);  // 挂起线程
		num = tmp;
		printf("Thread A, id=%ld, i=%d, number=%d\n", pthread_self(), i, num);
	}

	return NULL;
}

void* funB(void* arg)
{
	for (int i = 0; i < MAX; ++i)
	{
		int tmp = num;
		++tmp;
		num = tmp;
		printf("Thread B, id=%ld, i=%d, number=%d\n", pthread_self(), i, num);
		usleep(5);
	}

	return NULL;
}

int main(void)
{
	pthread_t p1, p2;

	pthread_create(&p1, NULL, funA, NULL);
	pthread_create(&p2, NULL, funB, NULL);

	pthread_join(p1, NULL);  // join会阻塞线程，直到子线程exit或执行完毕
	pthread_join(p2, NULL);
	
	return 0;
}
