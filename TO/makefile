CC=gcc
EXEC=to.exe
SOURCE=to.c clrscr.c

all:$(EXEC)

$(EXEC): $(SOURCE)
	$(CC) $^ -o $@ -lstdc++

clean:
	rm -rf $(EXEC)
	
