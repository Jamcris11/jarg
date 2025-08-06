
OUT=libjarg.so
SRC=src/arg.c

build: $(SRC)
	gcc -g -Iinclude -shared -fPIC -o $(OUT) $^ 

src/%.c:
	gcc -c -Wall -Werror -fPIC $@

install:
	cp $(OUT) /usr/lib/$(OUT)
	cp -r include/ /usr/include/jarg/
