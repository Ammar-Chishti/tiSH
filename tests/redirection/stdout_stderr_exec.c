#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Hello Stdout");
    fprintf(stderr, "Hello Stderr");
    return 0;
}
