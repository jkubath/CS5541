all: deadlock starvation
	gcc -Wall -std=c99 -o philosopher philosopher.c

use: 
	./philosopher

deadlock:
	gcc -Wall -std=c99 -o philosopher_deadlock philosopher_deadlock.c

use1: 
	./philosopher_deadlock

starvation:
	gcc -Wall -std=c99 -o philosopher_starvation philosopher_starvation.c

use2: 
	./philosopher_starvation