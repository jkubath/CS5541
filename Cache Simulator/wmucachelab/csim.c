/*
 * Name: Jonah Kubath
 * Email: jnn3350@wmich.edu
 * Assignment: Cache Simulator
 * Date: 2/22/2018
 *
 * Summary: Cache simulator.  This program reads in the data held in the trace
 * file made by valgrind.  The values are then entered into a "Cache" array
 * to simulate the loading, storing, and manipulating of data held in the cache.
 * The file will output the hits, misses, and miss/eviction count to the command
 * line along with creating the file .csim_results
 *
 * Trace commands default to an array of 1000, but is reallocated if needed.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

//Holds the data read from the trace file
struct line_t {
        char opp;
        unsigned long long addNumber;
        int size;
} command;

//Holds a location in the cache array
struct cache_t {
        struct line_t *tag;
        int valid;
        int used;
} cache;

//Function literals
int runCache(struct line_t *traceArray, struct cache_t **cacheArray,
             int *hit, int *miss, int *eviction, int *instructionCount);
int readFile(struct line_t **traceArray, int baseNumber, char *fileName);
void readArgs(char **argv, int argc, char **fileName,int *S, int *E, int *B);
void printSummary(int hits,  /* number of  hits */
                  int misses, /* number of misses */
                  int evictions); /* number of evictions */
int findFree(struct cache_t *cacheLine, struct line_t* command);
void addReturn(int *hit, int *miss, int *eviction, int *returnValue, int m);
int checkArgs();

/*
 * printSummary - Summarize the cache simulation statistics. Student cache simulators
 *                must call this function in order to be properly autograded.
 */
void printSummary(int hits, int misses, int evictions)
{
        printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
        FILE* output_fp = fopen(".csim_results", "w");
        assert(output_fp);
        fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
        fclose(output_fp);
}

int verbose = 0;
int S = -1;
int E = -1;
int B = -1;
unsigned long long mask = -1;
unsigned long long totalMask = -1;
int main(int argc, char **argv)
{
        int baseNumber = 1000;
        struct line_t *traceArray = calloc(baseNumber * sizeof(command), 1);
        int instructionCount = 0;
        char *fileName = NULL;
        int hit = 0;
        int miss = 0;
        int eviction = 0;

        //Read the arguments from the command line
        readArgs(argv, argc, &fileName, &S, &E, &B);

        //Find the mask for the Set bits
        mask = (mask << S) & (((unsigned) -1) >> (64 - S - B));
        totalMask = totalMask << (S + B);

        //Total number of sets
        S = 1 << S;

        //Print arguments the read
        if(verbose) {
                printf("S - %d\n", S);
                printf("E - %d\n", E);
                printf("B - %d\n", B);
        }

        if(checkArgs()) {
                //Exit as arguments are bad
                return 1;
        }
        // printf("Mask - %llx\n", mask);
        // printf("total Mask - %llx\n", totalMask);
        // printf("file - %s\n", fileName);

        struct cache_t **cacheArray = calloc(S * sizeof(cache), 1);
        //Initialize line of cache
        for(int i = 0; i < S; i++) {
                cacheArray[i] = calloc(E * sizeof(cache), 1);
                for(int j = 0; j < E; j++) {
                        cacheArray[i][j].tag = NULL;
                        cacheArray[i][j].valid = 0;
                        cacheArray[i][j].used = 0;
                }
        }

        //Read the instructions and return the count
        instructionCount = readFile(&traceArray, baseNumber, fileName);

        //Run through the instructions
        runCache(traceArray, cacheArray, &hit, &miss, &eviction, &instructionCount);

        //Print the summary to console
        printSummary(hit, miss, eviction);

        //Free data
        for(int i = 0; i < S; i++) {
                free(cacheArray[i]);
        }
        free(cacheArray);
        free(traceArray);
        free(fileName);
        return 0;
}

/*
 * Read the trace file as indicated from -t in the arguments
 */
int readFile(struct line_t **traceArrayOriginal, int baseNumber, char *fileName) {
        //Read from traces folder
        int lineSize = 100;
        FILE *file = fopen(fileName, "r");
        char *line = calloc(lineSize, 1);
        int count = 0;
        char *individual = NULL;
        int attCount = 0;
        int reallocCount = 1;
        struct line_t *traceArray = *traceArrayOriginal;

        while (fgets(line, lineSize, file) != NULL) {
                //printf("%s--End\n", line);
                individual = NULL;
                attCount = 0;

                //Break the line into the struct data
                individual = strtok(line, " ,");
                while(individual != NULL) {
                        // printf("Ind - %s\n", individual);
                        if(attCount == 0) {
                                strncpy(&traceArray[count].opp, individual, 1);
                        }
                        else if(attCount == 1) {
                                traceArray[count].addNumber = strtoll(individual, NULL, 16);
                                //printf("Double - %llx\n", traceArray[count].addNumber);
                        }
                        else if(attCount == 2) {
                                traceArray[count].size = atoi(individual);
                        }
                        else{
                                //Do nothing
                        }
                        individual = strtok(NULL, " ,");
                        attCount++;
                }
                // printf("Opp - %c\n", traceArray[count].opp);
                // printf("Add - %llx\n", traceArray[count].addNumber);
                // printf("Int - %d\n", traceArray[count].size);

                count++;

                //Need to allocate for more space
                if(count >= (baseNumber * reallocCount)) {
                        // printf("Realloc %d\n", reallocCount);
                        reallocCount++;
                        struct line_t *temp = realloc(*traceArrayOriginal, (baseNumber * sizeof(command)) * reallocCount);

                        if(temp == NULL) {
                                printf("Reallocation failed\n");
                        }
                        else {
                                *traceArrayOriginal = temp;
                                traceArray = *traceArrayOriginal;
                        }

                }

        }

        free(line);
        fclose(file);
        return count;
}

/*
 * Read the command line arguments and set values accordinly
 */
void readArgs(char **argv, int argc, char **fileName, int *S, int *E, int *B) {
        int count = 1;
        char *temp = NULL;
        while(count < argc) {
                temp = argv[count];
                if(strcmp(temp, "-s") == 0) {
                        *S = atoi(argv[count + 1]);
                        //*S = 2 ^ (*S);
                }
                else if(strcmp(temp, "-E") == 0) {
                        *E = atoi(argv[count + 1]);
                }
                else if(strcmp(temp, "-b") == 0) {
                        *B = atoi(argv[count + 1]);
                }
                else if(strcmp(temp, "-t") == 0) {
                        *fileName =(char *) calloc(strlen(argv[count + 1]) + 1, sizeof(char));
                        strncpy(*fileName, argv[count + 1], strlen(argv[count + 1]));
                }
                else if(strcmp(temp, "-v") == 0) {
                        verbose = 1;
                        count--;
                }
                else if(strcmp(temp, "-h") == 0) {
                        printf("./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
                        count--;
                }
                else {
                        //Do nothing
                        printf("Unknown argument found [%s]\n", argv[count]);
                }

                count += 2;
        }


}

/*
 * Run through the commands read in by the trace file.
 */
int runCache(struct line_t *traceArray, struct cache_t **cacheArray,
             int *hit, int *miss, int *eviction, int *instructionCount) {

        int set = 0;
        int returnValue = 0;
        //Run through each command
        for(int i = 0; i < *instructionCount; i++) {
                if(verbose && traceArray[i].opp != 'I')
                        printf("%c %llx,%d ", traceArray[i].opp, traceArray[i].addNumber, traceArray[i].size);

                //Hold the return value from findFree function
                returnValue = 0;

                // printf("%d - %c %llx\n", i, traceArray[i].opp, traceArray[i].addNumber);

                //Find the set to enter the number in
                set = (traceArray[i].addNumber & mask) >> B;
                //set = (traceArray[i].addNumber >> B) & 0x3;
                // printf("Set - %d\n", set);

                if(traceArray[i].opp == 'L') {
                        returnValue = findFree(cacheArray[set], &traceArray[i]);
                }
                else if(traceArray[i].opp == 'M') {
                        returnValue = findFree(cacheArray[set], &traceArray[i]);
                        addReturn(hit, miss, eviction, &returnValue, 0);
                        returnValue = findFree(cacheArray[set], &traceArray[i]);
                }
                else if(traceArray[i].opp == 'S') {
                        returnValue = findFree(cacheArray[set], &traceArray[i]);
                }
                else if(traceArray[i].opp == 'I') {
                        //Do nothing
                        continue;
                }
                else {

                }

                addReturn(hit, miss, eviction, &returnValue, 1);

        }//End of for loop for running the commands


        return 1;
}

/*
 * Find a free position in the set to place the command
 * 0 - hit
 * 1 - miss
 * 2 - miss & eviction
 */
int findFree(struct cache_t *cacheLine, struct line_t* command){
        // printf("Command sent - %llx\n", command->addNumber);
        int position = 0;
        //Used to break if the number is found
        int found = -1;
        //Search the cacheLine until the tag is found, null position is hit, or end of line
        while(position < E && cacheLine[position].tag != NULL) {
                // printf("Compare - %llx\n", cacheLine[position].tag->addNumber);
                // printf("Position - %d\n", position);

                //The tag was found
                if((command->addNumber & totalMask) == (cacheLine[position].tag->addNumber & totalMask)) {

                        //Valid bit was set
                        if(cacheLine[position].valid) {
                                cacheLine[position].used = 0;
                                //return 0;
                                found = 0;
                        }
                        //Set the data to valid
                        else {
                                cacheLine[position].valid = 1;
                                cacheLine[position].used = 0;
                                //return 1;
                                found = 1;
                        }
                }
                else {
                        cacheLine[position].used++;
                }

                position++;
        }

        if(found == 0)
                return 0;
        else if(found == 1)
                return 1;

        //A null was found in
        if(position < E) {
                cacheLine[position].tag = command;
                cacheLine[position].valid = 1;
                cacheLine[position].used = 0;
                return 1;
        }
        //I need to find the highest used value to evict
        else {
                //printf("Find the least used\n");
                int evictPosition = 0;
                for(int i = 1; i < E; i++) {
                        if(cacheLine[i].used > cacheLine[evictPosition].used)
                                evictPosition = i;
                }

                cacheLine[evictPosition].tag = command;
                cacheLine[evictPosition].valid = 1;
                cacheLine[evictPosition].used = 0;
                return 2;

        }

        return 0;

}

/*
 * Based on the value of returnValue
 * 0 - hit
 * 1 - miss
 * 2 - miss and eviction
 */
void addReturn(int *hit, int *miss, int *eviction, int *returnValue, int m) {
        if(*returnValue == 0) {
                if(verbose)
                        printf("%s", "hit");
                *hit = *hit + 1;
        }
        else if(*returnValue == 1) {
                if(verbose)
                        printf("%s", "miss");
                *miss = *miss + 1;
        }
        else {
                if(verbose)
                        printf("%s", "miss eviction");
                *miss = *miss + 1;
                *eviction = *eviction + 1;
        }

        //If the M command is given, addReturn is called twice
        //To print the action in the same line do not print \n, but print a space
        if(verbose && m) {
                printf("\n");
        }
        else {
                printf(" ");
        }
}

/*
 * Check the arguments given, return 1 for error
 */
int checkArgs() {
        if(B < 0 || S < 0 || E < 0) {
                printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
                printf("Options:\n");
                printf("   -h\tPrint usage\n");
                printf("   -v\tOptional verbose flag\n");
                printf("   -s <num>\tNumber of set index bits\n");
                printf("   -E <num>\tNumber of lines per set\n");
                printf("   -b <num>\tNumber of block offset bits\n");
                printf("   -t <file>\tTrace file\n");
                printf("Examples:\n");
                printf("   linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");

                return 1;
        }
        //Set and bit take up the entire 64 bits
        else if((S + B) > 63) {
                printf("Set index and block size are too large\n");
                return 1;
        }

        return 0;
}
