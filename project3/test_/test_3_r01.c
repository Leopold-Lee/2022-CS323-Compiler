#include <stdio.h>
int write(int i) {
    printf("%d\n", i);
}
int read() {
    return 4;
}
int main()
{
    int a, b, c;
    int final = 0;
    a = 5;
    b = a * a * (a + 2);
    write(b);
    c = b / a + 1;
    write(c);
    final = a + b - c * 3 + (b / a - 4);
    write(final);
    return 0;
}
