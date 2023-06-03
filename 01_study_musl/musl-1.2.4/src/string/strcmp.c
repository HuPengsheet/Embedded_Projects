#include <string.h>


//比较两个字符串是否相等，当不等时返回，返回值为不等的那个字符串相减
int strcmp(const char *l, const char *r)
{
	for (; *l==*r && *l; l++, r++);
	return *(unsigned char *)l - *(unsigned char *)r;
}
