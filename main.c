#include <stdio.h>
#include <malloc.h>

#define MAX_REFERENCES 30

struct input {
    int frames;
    int referenceSize;
    int references[MAX_REFERENCES];
};

int readInput(struct input *input) {
    char inputFile[1024] = {0, };
    printf("Input file: ");
    scanf("%s", inputFile);

    FILE *fi = fopen(inputFile, "rt");
    if (fi == NULL) {
        printf("Could not open the input file\n");
        return -1;
    }

    fscanf(fi, "%d", &input->frames);
    int i;
    for (i = 0; i < MAX_REFERENCES; ++i) {
        if (fscanf(fi, "%d", &input->references[i]) == EOF) {
            break;
        }
    }
    input->referenceSize = i;
    fclose(fi);

    return 0;
}

int main() {
    int error;

    struct input input;
    if ((error = readInput(&input)) < 0) {
        return error;
    }

    return 0;
}
