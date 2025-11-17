//simple C strcpy() can cause a buffer overflow
#include <stdio.h>
#include <string.h>

void vulnerable_function(const char *input) {
    char buffer[16];
    // no bounds checking
    strcpy(buffer, input);  
    printf("You entered: %s\n", buffer);
}

int main() {
    char name[128];

    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);

    // Remove newline for simplicity
    name[strcspn(name, "\n")] = 0;

    vulnerable_function(name);
    return 0;
}

