all:
	gcc -Wall -g -std=c99 -o csim csim.c

use:
	./csim -v -s 2 -E 2 -b 3 -t traces/trans.trace

ref: csim-ref
	./csim-ref -v -s 2 -E 2 -b 3 -t traces/trans.trace
