all:count scheduler 

count:count.c
	gcc -o count count.c
		
scheduler:scheduler.c
	gcc -o scheduler scheduler.c
	sudo taskset --cpu-list 1 "./scheduler"

clean:
	rm -f count 
	rm -f scheduler