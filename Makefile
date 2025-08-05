
SRC=src/arg.c

build: $(SRC)
	gcc -Iinclude -shared -fPIC -o libjarg.so $^ 

src/%.c:
	gcc -c -Wall -Werror -fPIC $@
