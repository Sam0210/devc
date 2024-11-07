/* 用互斥锁锁定一个代码块，被锁定的这个代码块，所有线程只能顺序执行（不能并行处理）。
   --解决数据混乱问题，但代价是执行效率低。
   --一般，每一个共享资源对应一把互斥锁，互斥锁个数与线程数无关。
   --哪个线程加锁，哪个线程才能够解锁。  */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 50

pthread_mutex_t mutex;
int num;    // 存储类型为静态外部链接的变量/全局变量，自动赋初值，位于.bss段


void* funA(void* arg)
{
	for (int i = 0; i < MAX; ++i)
	{
		pthread_mutex_lock(&mutex);
		
		// --- 临界区（粒度越小越好）
		int cur = num;
		++cur;
		usleep(10);  // 强制挂起
		num = cur;
		// ---

		pthread_mutex_unlock(&mutex);
		printf("Thread A, i=%d, number=%d\n", i, num);
	}
	return NULL;
}


void* funB(void* arg)
{
	for (int i = 0; i < MAX; ++i)
	{
		pthread_mutex_lock(&mutex);

		// --- 临界区（粒度越小越好）
		int cur = num;
		++cur;
		num = cur;
		// ---

		pthread_mutex_unlock(&mutex);
		printf("Thread B, i=%d, number=%d\n", i, num);
		usleep(5);
	}
	return NULL;
}


int main(void)
{
	pthread_t p1, p2;

	pthread_mutex_init(&mutex, NULL);

	pthread_create(&p1, NULL, funA, NULL);
	pthread_create(&p2, NULL, funB, NULL);

	pthread_join(p1, NULL);  // 阻塞线程，直到子线程exit或执行完毕
	pthread_join(p2, NULL);

	pthread_mutex_destroy(&mutex);
	
	return 0;
}
