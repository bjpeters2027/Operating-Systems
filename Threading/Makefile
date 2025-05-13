all: prolific.c prolific generation generation.c explorer.c explorer
	make clean
	make pro
	make gen
	make exp
	make slug
	make slugrace
	@if [ $$? -eq 0 ]; then \
		clear; \
		echo "All files compiled!"; \
	fi

pro: prolific.c
	gcc -Wall -Werror prolific.c -o prolific || exit 1

gen: generation.c
	gcc -Wall -Werror generation.c -o generation || exit 1

exp: explorer.c
	gcc -Wall -Werror explorer.c -o explorer || exit 1

slug:
	gcc -Wall -Werror slug.c -o slug || exit 1

race: slugrace.c
	gcc -Wall -Werror slugrace.c -o slugrace || exit 1

clean: prolific generation explorer
	rm -f prolific generation explorer slug
