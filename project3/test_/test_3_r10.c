#include <stdio.h>
int write(int i) {
    printf("%d\n", i);
}
int read() {
    return 4;
}
int main()
{
    int n;
    n = read();
    if (n > 0) write(1);
    else if (n < 0) write (-1);
    else write(0);
    return 0;
}
