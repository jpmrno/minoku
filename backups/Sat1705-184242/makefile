TARGET = minoku
CC = gcc
CFLAGS = -Wall -g
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)
TIME = `date "+%a%d%m-%H%M%S"`

.PHONY: default all clean backup run

default:$(TARGET)

all:	default

%.o:	%.c $(HEADERS)
		$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS:	$(TARGET) $(OBJECTS)

$(TARGET):	$(OBJECTS)
			$(CC) $(OBJECTS) -Wall -o $@

clean:
		-rm -f *.o
		-rm -f $(TARGET)
		clear

backup:
		@echo Realizando Backup...
		@echo Verificando directorios...
		@mkdir -p backups/
		@mkdir backups/$(TIME)/
		@echo Copiando archivos...
		cp *.h backups/$(TIME)/
		cp *.c backups/$(TIME)/
		cp makefile backups/$(TIME)/
		@echo Backup completo!

run:
	clear
	@./minoku
