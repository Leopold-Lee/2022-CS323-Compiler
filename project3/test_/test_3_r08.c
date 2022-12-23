#include <stdio.h>
int write(int i) {
    printf("%d\n", i);
}
int read() {
    return 4;
}
int mod(int x,int n)
{
    return x -(x / n) * n;
}

int DigitSum(int y)
{
     if(y == 0)
        return 0;
     return mod(y, 10) + DigitSum(y / 10);
}

int main()
{
    int num;
    num = read();
    if(num < 0)
        write(-1);
    else 
        write(DigitSum(num));
    return 0;
}
