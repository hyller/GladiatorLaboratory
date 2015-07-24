CC=gcc
EXEC=win32_com.exe
SOURCE=win32_com.c

all:$(EXEC)

$(EXEC): $(SOURCE)
	$(CC) $^ -o $@ -lstdc++
	
clean:
	rm -rf $(EXEC)
	
