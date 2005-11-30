int NumDigits(int i)
{
int num;
    if (i<0) {
        printf("internal error: negative number passed to NumDigits\n");
	exit(1);
      }
    num = 0;
    while (i>0) {
        num++;
	i = i/10;
    }
    return(num);
}
char *IntToStr(int i)
{
char *s;
int len;
    if (i <= 0) {
        printf("internal error: non-positive number passed to IntToStr\n");
	exit(1);
    }
    len = NumDigits(i);
    s = (char *)malloc(len+1);
    s[len] = '\0';
    len--;
    while (len >= 0) {
        s[len] = i % 10 + '0';
	len--;
	i = i/10;
    }
    return(s);
}
