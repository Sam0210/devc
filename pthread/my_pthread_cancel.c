/*
  主线程调用cancel函数后，线程B并不是马上killed，其需要进行一次系统调用kill自己。
  这里，若pthread_cancel函数并不能完成预期任务（取消子线程），是因为printf函数不会被视为取消点。可以将printf更改为read、sleep、wait等明确被设定为取消点的函数。
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


void* callback(void* arg)
{
	int j = 0;
	for (int i = 0; i < 6; ++i)
		j++;
	
	printf("child thread id: %ld\n", pthread_self());
	for (int i = 0; i < 90; ++i) 
	{
		printf("child i = %d\n", i);  // printf可能由于缓存机制等等影响，不会被认为是取消点
		usleep(1);  // 明确的取消点（系统调用）
	}
	return NULL;
}


int main(void)
{
	pthread_t tid;
	pthread_create(&tid, NULL, callback, NULL);

	printf("child thread has been created successfully!\n");
	printf("main thread id: %ld\n", pthread_self());
	for (int i = 0; i < 3; ++i)
		printf("i = %d\n", i);
	
	pthread_cancel(tid);

	pthread_exit(NULL);

	return 0;
}
