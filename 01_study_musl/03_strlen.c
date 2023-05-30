#include<stdio.h>
#include<string.h>
#include<time.h>
#include <stdint.h>
#include <limits.h>



#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)
#define  N   1000000000
#define RUN_TIME(fun)   {\
                                                        start = time(NULL);\
                                                        fun;\
                                                        end = time(NULL);\
                                                        printf("%lf\n", difftime(end, start));\
                                                        }

#define RUN_TIME2(fun)   {\
                                                        start2 = clock();\
                                                        fun;\
                                                        end2 = clock();\
                                                        printf("%lf\n", ((double) (end2 - start2)) / CLOCKS_PER_SEC);\
                                                        }

//对标准库里面的strlen函数进行相关实验

char str1[N];
time_t start, end;
double time_used;
clock_t start2, end2;
double cpu_time_used;

//初始化字符串，即遍历字符数组赋值
void str_init();

void my_strlen();
size_t musl_strlen(const char *s);


int main(){
      
    
// printf("初始化使用的时间：");RUN_TIME2(str_init());
// printf("自己实现的函数使用时间：");RUN_TIME2(my_strlen());
// printf("GUN标准库函数使用时间：");RUN_TIME2(strlen(str1));  
//printf("musl标准库函数使用时间：");RUN_TIME2(musl_strlen(str1));
// printf("%lx\n",ONES);
// printf("%lx\n",HIGHS);
size_t x=0x3001220101010101;
printf("%d\n",!HASZERO(x));
printf("%lx\n",x-ONES);

printf("%lx\n",~x);
printf("%lx\n",(x)-ONES&~(x)&HIGHS);
printf("%lx\n",HIGHS);

    return 0;
}



void str_init(){
    for (int i = 0;i < N; i++) str1[i] = 'a';
}

void my_strlen(){
    const char * a=str1;
    for(;*a;a++);
    //printf("%lu\n",a-str1);
}



size_t musl_strlen(const char *s)
{
	const char *a = s;

	//检测速度优化代码
	typedef size_t __attribute__((__may_alias__)) word;
	const word *w;
	//首先让s指针，指向对其sizeof(size_t)的位置，这里面s还是一个一个加的
	//加到对齐的地方主要是为了后面的批量查找
	//如果在找对齐的期间，已经到了字符串的末尾的话，就直接返回
	for (; (uintptr_t)s % ALIGN; s++) if (!*s) return s-a;
	//!HASZERO(*w) -》(ONES * (UCHAR_MAX/2+1))(*w)
	for (w = (const void *)s; !HASZERO(*w); w++);
	s = (const void *)w;

	for (; *s; s++);
   // printf("%ld\n",s-a);
	return s-a;
}

