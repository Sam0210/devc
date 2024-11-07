/* 
  子线程与主线程分离，子线程退出后，其占用的内核资源自动被其他线程接管回收。
  detach不同于join，不会造成阻塞。
*/


#include <stdio.h>
#include <pthread.h>

void* working(void* arg)
{
	for (int i = 0; i < 3; ++i)
		printf("child i = %d\n", i);
	return NULL;
}


int main(void)
{
	pthread_t tid;
	pthread_create(&tid, NULL, working, NULL);
	
	pthread_detach(tid);  // 不阻塞线程，该线程继续向下执行
	
	for (int i = 0; i < 3; ++i)
		printf("i = %d\n", i);	

	pthread_exit(NULL);
	return 0;		
}
