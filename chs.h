/*
 *  name:   chs.h
 *  func:   char* strcopy (char *ch)
 *  func:   void del_space (char *ch)
 *
 */
#include <string.h>
char* strcopy (char *ch) {
	int leng = strlen (ch);
	char *t = (char *)malloc (leng+1);
	char *rp = t;
	while ((*t++ = *ch++)!='\0');	//while (*t++ = *ch++);
	return rp;
}

void my_to_lower (char *ch) {
	for(; *ch; ch++)
		if (*ch >= 'A' && *ch <= 'Z')
			*ch += 32;	//or 0x20 or 'a' - 'A'
}

void del_space (char *ch) {
	int i;  //第一源下标
	int j;  //修正后的目标下标
	int k;  //第二源下标

	//去掉头部的空格
	for (i=0; ch[i]==' '; i++);
	for (j=0; (ch[j]=ch[i]); i++,j++);


	//去掉尾部的空格
	for (i=strlen (ch)-1; ch[i]==' '; i--);
	ch[i+1] = '\0';

	//去掉中间多余的空格
	for (i=0; ch[i]; i++) {
		if (ch[i] == ' ') {
			for (j=k=i+1; ch[k] == ' '; k++);
			for (; (ch[j]=ch[k]); j++,k++);
		}
	}
	return ;
}

