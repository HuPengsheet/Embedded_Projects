#include <string.h>
#include <stdint.h>
#include <limits.h>

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)             //ONES=0x 01 01 01 01 01 01 01 01
#define HIGHS (ONES * (UCHAR_MAX/2+1))         //HIGHS=0X 80 80 80 80 80 80 80 80
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)   //HIGHS：只看每个字节的最高位是 ->~(x)：最高位必须是0，取反后才是1
											   //->  (x)-ONES:x减去ONES后，最高位必须是1才可以
											   //三个条件其实综合起来就是，每个字节的最高位是0，减了ONES后变成1，只有00，01才符合上面的三个条件
												//但是01的时候，说明前面也有00
												//需要注意的，只能看最高位的变化，比如你看0-7的第六位，1000000-1的时候，第六位原来是0，减了以后变成了1
												//



size_t strlen(const char *s)
{
	const char *a = s;
#ifdef __GNUC__
	//检测速度优化代码
	typedef size_t __attribute__((__may_alias__)) word;
	const word *w;
	//首先让s指针，指向对其sizeof(size_t)的位置，这里面s还是一个一个加的
	//加到对齐的地方主要是为了后面的批量查找
	//如果在找对齐的期间，已经到了字符串的末尾的话，就直接返回
	for (; (uintptr_t)s % ALIGN; s++) if (!*s) return s-a;
	//!HASZERO(*w) -》(ONES * (UCHAR_MAX/2+1))(*w)

	//每size_t个字节来搜素，以此提高效率，HASZERO用来查看8个字节里有没有NULL字符
	for (w = (const void *)s; !HASZERO(*w); w++);
	s = (const void *)w;
#endif
	//最后再8个里面字符里面一个一个查找
	for (; *s; s++);
	return s-a;
}
