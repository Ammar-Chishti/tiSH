#include <stdio.h>
#include <unistd.h>

int main() {
    char buff[getpagesize()];
    read(STDIN_FILENO, buff, 17);
    buff[17] = '\0';

    printf("%s", buff);
    fprintf(stderr, "Hello Stderr");
    return 0;
}
