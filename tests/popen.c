#include <stdio.h>

int main() {
    FILE* file = popen("uname", "r");
    char buffer[101];
    fscanf(file, "%100s", buffer);
    pclose(file);
    printf("%s\n", buffer);
    return 0;
}
