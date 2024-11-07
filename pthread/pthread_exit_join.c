/* 
  调用exit当前线程就立即退出，且不会影响其他线程，无论是在主线程还是子线程中。（安全退出）
  join会引起调用者阻塞，一次回收一个子线程，且不能多次回收（耍流氓）
  子线程exit时，通过位于全局、主线程栈、堆中的变量，向join传值！（子线程栈中变量不行，因为其会被释放）
  ---多线程独享：自己的栈区、寄存器（内核中）
  ---多线程共享：代码区、堆区、全局变量区、文件描述符表
 */


#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define USE_MALLOC_VAR  // 堆变量向join传值
//#define USE_GLOBAL_VAR  // 全局变量向join传值
// #define USE_MAIN_VAR    // 主线程栈变量向join传值

struct Person {
	char name[36];
	int age;
	unsigned int id;
};

#ifdef USE_GLOBAL_VAR
struct Person p;  // 必要的
struct Person* ps = &p;
#endif


void* callback(void* arg)
{
#ifdef USE_MAIN_VAR
	struct Person* ps = (struct Person*)arg;
#endif
#ifdef USE_MALLOC_VAR
	struct Person* ps = (struct Person*)malloc(sizeof(struct Person));
#endif

	printf("Child Thread id : %ld\n", pthread_self());
	for (int i = 0; i < 6; ++i)
	{
		printf("Child == i := %d\n", i);
		if (i == 4)
		{
			strcpy(ps->name, "FXC");
			ps->age = -100;
			ps->id = 666;
			pthread_exit(ps);
		}
	}
	return NULL;
}


int main(void)
{
	pthread_t tid;

#ifdef USE_MAIN_VAR
	struct Person ps;
	pthread_create(&tid, NULL, callback, &ps);	
#endif
#if (defined USE_GLOBAL_VAR) || (defined USE_MALLOC_VAR)
	pthread_create(&tid, NULL, callback, NULL);	
#endif

	printf("Child thread has been created successfully!\n");
	printf("Main Thread id : %ld\n", pthread_self());
	for (int i = 0; i < 3; ++i) {
		printf("i = %d\n", i);		
	}

	void* ptr = NULL;
	pthread_join(tid, &ptr);
	struct Person* p = (struct Person*)ptr;
	printf("name:%s\tage:%d\tid:%d\n", p->name, p->age, p->id);
#ifdef USE_MALLOC_VAR
	free(p);
#endif
	return 0;
}
