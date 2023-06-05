

# musl库学习

## 配置与编译

### configure文件

根文件夹下有两个相关文件，分别是configure和Makefile文件夹

![image-20230605110545804](C:\Users\hupeng\Desktop\开源项目维护\01_C标准库\image-20230605110545804.png)

​	首先使用shell执行configure生成配置文件

```shell
./configure
```

​	命令执行完成后会生成config.mak文件,使用vim编辑器打开可以看到相关配置信息，ARCH=x86_64，CC=gcc等。显然ARCH是musl库编译的目标架构，CC是要使用的编译器。

![image-20230605111347791](C:\Users\hupeng\Desktop\开源项目维护\01_C标准库\2)

​	进一步探究configure文件的内容，打开configure我们可以看到

![image-20230605112159316](C:\Users\hupeng\Desktop\开源项目维护\01_C标准库\image-20230605112159316.png)

这些变量在我们执行./configure时是可以指定的，如果我们没有制定的话，configure也会通过一些命令生成。以ARCH为例

![image-20230605112438776](C:\Users\hupeng\AppData\Roaming\Typora\typora-user-images\image-20230605112438776.png)

configure脚本里面有

```shell
test -n "$target" || target=$($CC -dumpmachine 2>/dev/null) || target=unknown
```

如果target没有定义的话，就会执行，$CC -dumpmachine命令，对应的也就是gcc -dumpmachine

```shelll
$ gcc -dumpmachine
输出结果： x86_64-linux-gnu
```

因此ARCH就会被设置为ARCH=x86_64

### Makefile文件



## string.h头文件

### memcpy

内存拷贝函数从src拷贝n个字节去dest。返回dest。

```c
#include <string.h>
#include <stdint.h>
#include <endian.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;

#ifdef __GNUC__

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define LS >>
#define RS <<
#else
#define LS <<
#define RS >>
#endif

	typedef uint32_t __attribute__((__may_alias__)) u32;
	uint32_t w, x;

	for (; (uintptr_t)s % 4 && n; n--) *d++ = *s++;   //要么已经拷贝完了，要么是s已经对齐4字节的地址了


	//d也对齐了4地址的话
	if ((uintptr_t)d % 4 == 0) {
		for (; n>=16; s+=16, d+=16, n-=16) {
			*(u32 *)(d+0) = *(u32 *)(s+0);
			*(u32 *)(d+4) = *(u32 *)(s+4);
			*(u32 *)(d+8) = *(u32 *)(s+8);
			*(u32 *)(d+12) = *(u32 *)(s+12);   //分四次，每次u32，4个字节，共拷贝16个字节
		}   //循环出来后，最后肯定剩下小于16个字节的数据需要拷贝，n<16

		//以13为例，也就是说还需要在拷贝13个字符串从s到d
		//13 = 1011=8+4+1
		//分别会进入对于的if语句里 n&8 n&4  n&1  最后再返回
		if (n&8) {
			*(u32 *)(d+0) = *(u32 *)(s+0);
			*(u32 *)(d+4) = *(u32 *)(s+4);
			d += 8; s += 8;
		}  
		if (n&4) {
			*(u32 *)(d+0) = *(u32 *)(s+0);
			d += 4; s += 4;
		}
		if (n&2) {
			*d++ = *s++; *d++ = *s++;
		}
		if (n&1) {
			*d = *s;
		}

		return dest;
	}


	//d不对齐了4地址的话
	if (n >= 32) switch ((uintptr_t)d % 4) {
	case 1:
		w = *(u32 *)s;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		n -= 3;
		//此时d对齐了
		//s没对齐了

		for (; n>=17; s+=16, d+=16, n-=16) {
			x = *(u32 *)(s+1);   //x是对齐字节的，下面的w也是对齐字节的
			*(u32 *)(d+0) = (w LS 24) | (x RS 8);   //取w的四字节，左移24位，低8位变高八位，在于x的32位右移8位（高八位就清零了），两个拼起来
			//实际呢就是利用这个拼接操作，让取值的时候指针是对齐字节的
			w = *(u32 *)(s+5);   //
			*(u32 *)(d+4) = (x LS 24) | (w RS 8);
			x = *(u32 *)(s+9);
			*(u32 *)(d+8) = (w LS 24) | (x RS 8);
			w = *(u32 *)(s+13);
			*(u32 *)(d+12) = (x LS 24) | (w RS 8);
		}
		break;
	case 2:
		w = *(u32 *)s;
		*d++ = *s++;
		*d++ = *s++;
		n -= 2;
		for (; n>=18; s+=16, d+=16, n-=16) {
			x = *(u32 *)(s+2);
			*(u32 *)(d+0) = (w LS 16) | (x RS 16);
			w = *(u32 *)(s+6);
			*(u32 *)(d+4) = (x LS 16) | (w RS 16);
			x = *(u32 *)(s+10);
			*(u32 *)(d+8) = (w LS 16) | (x RS 16);
			w = *(u32 *)(s+14);
			*(u32 *)(d+12) = (x LS 16) | (w RS 16);
		}
		break;
	case 3:
		w = *(u32 *)s;
		*d++ = *s++;
		n -= 1;
		for (; n>=19; s+=16, d+=16, n-=16) {
			x = *(u32 *)(s+3);
			*(u32 *)(d+0) = (w LS 8) | (x RS 24);
			w = *(u32 *)(s+7);
			*(u32 *)(d+4) = (x LS 8) | (w RS 24);
			x = *(u32 *)(s+11);
			*(u32 *)(d+8) = (w LS 8) | (x RS 24);
			w = *(u32 *)(s+15);
			*(u32 *)(d+12) = (x LS 8) | (w RS 24);
		}
		break;
	}
	if (n&16) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n&8) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n&4) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n&2) {
		*d++ = *s++; *d++ = *s++;
	}
	if (n&1) {
		*d = *s;
	}
	return dest;
#endif

	for (; n; n--) *d++ = *s++;
	return dest;
}

```

### memmove

与不同memcpy不同的是，memmove可以处理地址空间有重复的问题。

```c
#include <string.h>
#include <stdint.h>

#ifdef __GNUC__
typedef __attribute__((__may_alias__)) size_t WT;
#define WS (sizeof(WT))
#endif

void *memmove(void *dest, const void *src, size_t n)
{
	char *d = dest;
	const char *s = src;

	if (d==s) return d;    //相同就不用拷贝呀，笨蛋
	if ((uintptr_t)s-(uintptr_t)d-n <= -2*n) return memcpy(d, s, n);   //判断是否有重叠的地方，没有的话直接调用memcpy

	if (d<s) {
#ifdef __GNUC__
		if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
			while ((uintptr_t)d % WS) {
				if (!n--) return dest;
				*d++ = *s++;
			}
			for (; n>=WS; n-=WS, d+=WS, s+=WS) *(WT *)d = *(WT *)s;
		}
#endif
		for (; n; n--) *d++ = *s++;
	} else {
#ifdef __GNUC__
		if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
			while ((uintptr_t)(d+n) % WS) {
				if (!n--) return dest;
				d[n] = s[n];
			}
			while (n>=WS) n-=WS, *(WT *)(d+n) = *(WT *)(s+n);
		}
#endif
		while (n) n--, d[n] = s[n];
	}

	return dest;
}

```

### memset

批量设置dest地址，为值c。

```c
#include <string.h>
#include <stdint.h>

void *memset(void *dest, int c, size_t n)
{
	unsigned char *s = dest;
	size_t k;

	/* Fill head and tail with minimal branching. Each
	 * conditional ensures that all the subsequently used
	 * offsets are well-defined and in the dest region. */

	if (!n) return dest;
	s[0] = c;
	s[n-1] = c;
	if (n <= 2) return dest;
	s[1] = c;
	s[2] = c;
	s[n-2] = c;
	s[n-3] = c;
	if (n <= 6) return dest;
	s[3] = c;
	s[n-4] = c;
	if (n <= 8) return dest;

	/* Advance pointer to align it at a 4-byte boundary,
	 * and truncate n to a multiple of 4. The previous code
	 * already took care of any head/tail that get cut off
	 * by the alignment. */

	k = -(uintptr_t)s & 3;
	s += k;
	n -= k;
	n &= -4;

#ifdef __GNUC__
	typedef uint32_t __attribute__((__may_alias__)) u32;
	typedef uint64_t __attribute__((__may_alias__)) u64;

	u32 c32 = ((u32)-1)/255 * (unsigned char)c;

	/* In preparation to copy 32 bytes at a time, aligned on
	 * an 8-byte bounary, fill head/tail up to 28 bytes each.
	 * As in the initial byte-based head/tail fill, each
	 * conditional below ensures that the subsequent offsets
	 * are valid (e.g. !(n<=24) implies n>=28). */

	*(u32 *)(s+0) = c32;
	*(u32 *)(s+n-4) = c32;
	if (n <= 8) return dest;
	*(u32 *)(s+4) = c32;
	*(u32 *)(s+8) = c32;
	*(u32 *)(s+n-12) = c32;
	*(u32 *)(s+n-8) = c32;
	if (n <= 24) return dest;
	*(u32 *)(s+12) = c32;
	*(u32 *)(s+16) = c32;
	*(u32 *)(s+20) = c32;
	*(u32 *)(s+24) = c32;
	*(u32 *)(s+n-28) = c32;
	*(u32 *)(s+n-24) = c32;
	*(u32 *)(s+n-20) = c32;
	*(u32 *)(s+n-16) = c32;

	/* Align to a multiple of 8 so we can fill 64 bits at a time,
	 * and avoid writing the same bytes twice as much as is
	 * practical without introducing additional branching. */

	k = 24 + ((uintptr_t)s & 4);
	s += k;
	n -= k;

	/* If this loop is reached, 28 tail bytes have already been
	 * filled, so any remainder when n drops below 32 can be
	 * safely ignored. */

	u64 c64 = c32 | ((u64)c32 << 32);
	for (; n >= 32; n-=32, s+=32) {
		*(u64 *)(s+0) = c64;
		*(u64 *)(s+8) = c64;
		*(u64 *)(s+16) = c64;
		*(u64 *)(s+24) = c64;
	}
#else
	/* Pure C fallback with no aliasing violations. */
	for (; n; n--, s++) *s = c;
#endif

	return dest;
}
```

### memcmp

比较两段内存是否相等，相等返回0，不等的话返回对应的字节相减

```c
#include <string.h>

int memcmp(const void *vl, const void *vr, size_t n)
{
	const unsigned char *l=vl, *r=vr;
	for (; n && *l == *r; n--, l++, r++);
	return n ? *l-*r : 0;
}
```

### memchr

字符查询函数，查询是否含有字符c，如果有返回对应的地址，没有的话返回0

```c
#include <string.h>
#include <stdint.h>
#include <limits.h>

#define SS (sizeof(size_t))
#define ALIGN (sizeof(size_t)-1)
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

void *memchr(const void *src, int c, size_t n)
{
	const unsigned char *s = src;
	c = (unsigned char)c;
#ifdef __GNUC__
	for (; ((uintptr_t)s & ALIGN) && n && *s != c; s++, n--);   //字节对齐
	if (n && *s != c) {    //判断还没有结束
		typedef size_t __attribute__((__may_alias__)) word;   
		const word *w;
		size_t k = ONES * c;   
		for (w = (const void *)s; n>=SS && !HASZERO(*w^k); w++, n-=SS);   
        //批量查询，优化的HASZERO，其实是看有没有c																 
        //*w^k 表示与k异或，如何和k完全相同，那么就是0，也就可以使用HASZERO
		s = (const void *)w;
	}
#endif
	for (; n && *s != c; s++, n--);
	return n ? (void *)s : 0;
}

```



### strcpy与strncpy

strcpy是字符串拷贝函数，传入两个字符串指针src和dest，把src的字符串拷贝到dest里面去，返回的是dest。里面上实际上调用的是__stpcpy函数。

```c


#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)             //ONES=0x 01 01 01 01 01 01 01 01
#define HIGHS (ONES * (UCHAR_MAX/2+1))         //HIGHS=0X 80 80 80 80 80 80 80 80
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)   //x里面是否有字符0，如果有返回1，没有返回0

char *strcpy(char *restrict dest, const char *restrict src)
{
	__stpcpy(dest, src);  //调用__stpcpy函数
	return dest;
}

char *__stpcpy(char *restrict d, const char *restrict s)
{
#ifdef __GNUC__
	typedef size_t __attribute__((__may_alias__)) word;  //消除警告
	word *wd;
	const word *ws;
    //(uintptr_t)s % ALIGN == (uintptr_t)d % ALIGN 余数必须匹配上才行，不然无法同步，因为这是拷贝，指针必须同加
	if ((uintptr_t)s % ALIGN == (uintptr_t)d % ALIGN) {
		for (; (uintptr_t)s % ALIGN; s++, d++)
			if (!(*d=*s)) return d;      //将s和d的内存和ALIGN对齐，即能被整除，这样后面批量转换的时候可以提速
        								//中途如果已经到了结尾的话，就直接返回d
		wd=(void *)d; ws=(const void *)s;
		for (; !HASZERO(*ws); *wd++ = *ws++);      //批量拷贝，直到HASZERO(*ws)为1，即到字符串的结尾
		d=(void *)wd; s=(const void *)ws;          //赋值转换，兼容后面的代码
	}
#endif
	for (; (*d=*s); s++, d++);

	return d;
}
```

strcpy是字符串拷贝函数，传入两个字符串指针src和dest，和拷贝的字符的数量，把src的字符串前n个拷贝到dest里面去，返回的是dest。里面上实际上调用的是__stpncpy函数。如果n大于了src的长度的话，不够的话用memset函数填充0。

```c
#define ALIGN (sizeof(size_t)-1)        // 32为系统 4-1=3 0x11   64位  8-1=7 0x111
#define ONES ((size_t)-1/UCHAR_MAX)      //ONES=0x 01 01 01 01 01 01 01 01
#define HIGHS (ONES * (UCHAR_MAX/2+1))     //HIGHS=0X 80 80 80 80 80 80 80 80
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)     //x里面是否有字符0，如果有返回1，没有返回0

char *strncpy(char *restrict d, const char *restrict s, size_t n)
{
	__stpncpy(d, s, n);
	return d;
}

char *__stpncpy(char *restrict d, const char *restrict s, size_t n)
{
#ifdef __GNUC__
	typedef size_t __attribute__((__may_alias__)) word;
	word *wd;
	const word *ws;
    
    //(uintptr_t)s & ALIGN) 使用位运算来加快速度，不用取模来计算
    //例如ALIGN=1，对于的二进制为0100
    //s&0111==1 的充要条件是 s的后3位二进制是1，也就可以判断是不是对齐了
	if (((uintptr_t)s & ALIGN) == ((uintptr_t)d & ALIGN)) {
		for (; ((uintptr_t)s & ALIGN) && n && (*d=*s); n--, s++, d++);
		if (!n || !*s) goto tail;  //n个拷贝完了，或者s到头了，跳到tail
		wd=(void *)d; ws=(const void *)s;   //开始批量拷贝
		for (; n>=sizeof(size_t) && !HASZERO(*ws);
		       n-=sizeof(size_t), ws++, wd++) *wd = *ws;
		d=(void *)wd; s=(const void *)ws;
	}
#endif
	for (; n && (*d=*s); n--, s++, d++);
tail:
	memset(d, 0, n);//如果n大于了src的长度的话，不够的地方用memset函数填充0。
	return d;
}
```

### strcat和strncat

字符串拼接，返回dest地址

```c
char *strcat(char *restrict dest, const char *restrict src)
{
	strcpy(dest + strlen(dest), src);
	return dest;
}



char *strncat(char *restrict d, const char *restrict s, size_t n)
{
	char *a = d;
	d += strlen(d);
	while (n && *s) n--, *d++ = *s++;
	*d++ = 0;  //给字符串添加尾巴
	return a;
}

```

### strcmp和strncmp

相等时返回0，不等时返回字符相减

```c
//比较两个字符串是否相等，当不等时返回，返回值为不等的那个字符串相减
int strcmp(const char *l, const char *r)
{
	for (; *l==*r && *l; l++, r++);
	return *(unsigned char *)l - *(unsigned char *)r;
}



int strncmp(const char *_l, const char *_r, size_t n)
{
	const unsigned char *l=(void *)_l, *r=(void *)_r;
	//if (!n--) return 0; --为后置表达式，先判断为！n后，在n--
	if (!n--) return 0;
	for (; *l && *r && n && *l == *r ; l++, r++, n--);
	return *l - *r;
}
```



### strchr和strrchr

```c
char *strchr(const char *s, int c)
{
	char *r = __strchrnul(s, c);
	return *(unsigned char *)r == (unsigned char)c ? r : 0;
}

```



