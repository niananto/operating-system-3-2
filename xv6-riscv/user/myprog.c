#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    printf("Enter a number ");

    char buf[10];
    gets(buf, 9);
    int num = atoi(buf);
    printf("You entered %d\n", num);

    return 0;
}