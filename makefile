

autograde: autograde.c
	gcc autograde.c -o autograde

clean:
	rm autograde

install:
	cp autograde /usr/local/bin/autograde