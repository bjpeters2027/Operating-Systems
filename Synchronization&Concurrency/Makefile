all:
	make clean
	make field
	@if [ $$? -eq 0 ]; then \
		clear; \
		echo "All files compiled!"; \
	fi

field: field.c
	gcc -Wall field.c -o field || exit 1

clean:
	rm -f field
