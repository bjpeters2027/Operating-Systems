all:
	make clean
	make sched
	@if [ $$? -eq 0 ]; then \
		clear; \
		echo "All files compiled!"; \
	fi

sched:
	gcc -Wall scheduler.c -o scheduler || exit 1

clean:
	rm scheduler