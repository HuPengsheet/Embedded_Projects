#include<stdio.h>
#include<stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

/*          Start Addr           End Addr       Size     Offset objfile
      0x5555 5555 4000     0x555555555000     0x1000        0x0 /home/hupeng/code_c/c_test/c_lib_study/a.out
      0x555555754000     0x555555755000     0x1000        0x0 /home/hupeng/code_c/c_test/c_lib_study/a.out
      0x555555755000     0x555555756000     0x1000     0x1000 /home/hupeng/code_c/c_test/c_lib_study/a.out
      0x7ffff79e2000     0x7ffff7bc9000   0x1e7000        0x0 /lib/x86_64-linux-gnu/libc-2.27.so
      0x7ffff7bc9000     0x7ffff7dc9000   0x200000   0x1e7000 /lib/x86_64-linux-gnu/libc-2.27.so
      0x7ffff7dc9000     0x7ffff7dcd000     0x4000   0x1e7000 /lib/x86_64-linux-gnu/libc-2.27.so
      0x7ffff7dcd000     0x7ffff7dcf000     0x2000   0x1eb000 /lib/x86_64-linux-gnu/libc-2.27.so
      0x7ffff7dcf000     0x7ffff7dd3000     0x4000        0x0 
      0x7ffff7dd3000     0x7ffff7dfc000    0x29000        0x0 /lib/x86_64-linux-gnu/ld-2.27.so
      0x7ffff7fb4000     0x7ffff7fb6000     0x2000        0x0 
      0x7ffff7ff7000     0x7ffff7ffa000     0x3000        0x0 [vvar]
      0x7ffff7ffa000     0x7ffff7ffc000     0x2000        0x0 [vdso]


*/
//void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

int main(){
	
	//mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
	int file_ptr = open("he",O_RDONLY);
	int a =mmap((void*)0x555555554000, 0x3000, PROT_WRITE, MAP_FIXED, file_ptr ,0);
	printf("%d\n",a);
	printf("hello world\n");


	return 0;	
}
