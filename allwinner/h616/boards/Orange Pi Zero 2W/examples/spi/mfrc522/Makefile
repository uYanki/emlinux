CC=g++
CFLAGS=-std=c++11
EXTRA_LIBS = -lwiringPi 
DIR_OBJ=obj/
EXEC=app
 
all: $(EXEC)  clean
 
app: mfrc522.o main.o 
	$(CC) -o $@ $^ $(CFLAGS) $(EXTRA_LIBS)
	
%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(EXTRA_LIBS)
	
clean:
	@rm -f *.o
