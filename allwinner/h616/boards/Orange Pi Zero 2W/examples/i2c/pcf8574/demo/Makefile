# 若使用 %.h, 源文件就必须有同名的头文件, 否则无法编译

CC       := cc
CFLAGS   := -c -O2 -Wall 
LDFLAGS  := -lwiringPi -lwiringPiDev -lwiringPiLCD1602pi
SOURCES  := main.c

OBJECTS=$(SOURCES:%.c=../obj/%.o)
# HEADERS=$(SOURCES:%.c=%.h)
EXECUTABLE=app

all:   $(SOURCES) $(EXECUTABLE)
tasks: all

$(EXECUTABLE): $(OBJECTS) 
	@echo Compiling $@
	@mkdir -p ../bin
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@


# ../obj/%.o: %.h %.c
../obj/%.o: %.c
	@echo compiling $(*D)/$(*F).c
	@mkdir -p ../obj
	@$(CC) $(CFLAGS) $(*D)/$(*F).c -o $@


clean:
	@rm -rf ../obj/ $(EXECUTABLE)

