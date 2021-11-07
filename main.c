#include <stdio.h>
#include <malloc.h>
#include <memory.h>

#define MAX_FRAMES 4
#define MAX_REFERENCES 30

struct input {
    int frameSize;
    int referenceSize;
    int references[MAX_REFERENCES];
};

struct frameStatus {
    int frames[MAX_FRAMES];
    int hasFault;
};

struct output {
    struct input input;

    char usedMethod[10];
    struct frameStatus frameStatuses[MAX_REFERENCES];
};

int readInput(struct input *input) {
    char inputFile[1024] = {0,};
    printf("Input file: ");
    scanf("%s", inputFile);

    FILE *fi = fopen(inputFile, "rt");
    if (fi == NULL) {
        printf("Could not open the input file\n");
        return -1;
    }

    fscanf(fi, "%d", &input->frameSize);
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

int hasCached(int frameSize, const int frames[], int reference) {
    for (int i = 0; i < frameSize; ++i) {
        if (frames[i] == reference) {
            return 1;
        }
    }

    return 0;
}

int pickRoom(int frameSize, const int frames[])
{
    for (int i = 0; i < frameSize; ++i) {
        if (frames[i] == 0) {
            return i;
        }
    }

    return -1;
}

int optPickVictim(int frameSize, const int frames[], const int references[], int start, int end) {
    int referenceAts[MAX_FRAMES] = {0,};

    for (int i = 0; i < frameSize; ++i) {
        int j;
        for (j = start; j < end; ++j) {
            if (frames[i] == references[j]) {
                referenceAts[i] = j;
                break;
            }
        }
        referenceAts[i] = referenceAts[i] ? referenceAts[i] : j;
    }

    int victimIndex = -1;
    int max = 0;
    for (int i = 0; i < frameSize; ++i) {
        if (referenceAts[i] > max) {
            victimIndex = i;
            max = referenceAts[i];
        }
    }

    return victimIndex;
}

void simulateOpt(struct input input, struct output *output) {
    sprintf(output->usedMethod, "OPT");

    int currentFrames[MAX_FRAMES] = {0,};
    for (int i = 0; i < input.referenceSize; i++) {
        if (hasCached(input.frameSize, currentFrames, input.references[i]) == 1) {
            memcpy(output->frameStatuses[i].frames, currentFrames, sizeof(currentFrames));
            output->frameStatuses[i].hasFault = 0;
        } else {
            int victim;
            victim = pickRoom(input.frameSize, currentFrames);
            if (victim == -1) {
                victim = optPickVictim(input.frameSize, currentFrames, input.references, i + 1, input.referenceSize);
            }
            currentFrames[victim] = input.references[i];

            memcpy(output->frameStatuses[i].frames, currentFrames, sizeof(currentFrames));
            output->frameStatuses[i].hasFault = 1;
        }
    }
}

void printOutput(struct output output) {
    printf("Used method : %s\n", output.usedMethod);

    printf("page reference string : ");
    for (int i = 0; i < output.input.referenceSize; ++i) {
        printf("%d ", output.input.references[i]);
    }
    printf("\n");

    printf("\t\tframe\t");
    for (int i = 0; i < output.input.frameSize; ++i) {
        printf("%d\t\t", i + 1);
    }
    printf("page fault\n");

    printf("time\n");

    int cntPageFault = 0;
    for (int i = 0; i < output.input.referenceSize; ++i) {
        printf("%d\t\t\t\t", i + 1);
        for (int j = 0; j < output.input.frameSize; ++j) {
            if (output.frameStatuses[i].frames[j] == 0) {
                printf("\t\t");
            } else {
                printf("%d\t\t", output.frameStatuses[i].frames[j]);
            }
        }
        if (output.frameStatuses[i].hasFault == 1) {
            cntPageFault++;
            printf("F");
        }
        printf("\n");
    }

    printf("Number of page faults : %d times\n", cntPageFault);
}

int main() {
    int error;

    struct input input;
    if ((error = readInput(&input)) < 0) {
        return error;
    }

    struct output output;
    output.input = input;
    simulateOpt(input, &output);
    printOutput(output);

    return 0;
}
