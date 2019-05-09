CC=gcc

UTIL_OBJ=util.o

SERIAL_SORT_OBJ=${UTIL_OBJ} serial_sort.o

MAIN_SERIAL_OBJ=main_serial.o

OUTPUT_FOLDER=../TEMP

all: main_serial

main_serial: ${MAIN_SERIAL_OBJ} ${SERIAL_SORT_OBJ}
	${CC} $^ -o ${OUTPUT_FOLDER}/$@

*.o: *.c
	${CC} $^ -c

.PHONY clean:
	rm *.o