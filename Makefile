twst:
	gcc test.c -o test -I sqlite3 -L sqlite3 -lsqlite3 -lpthread -ldl

clean:
	rm test
