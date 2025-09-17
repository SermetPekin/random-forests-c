CC = gcc
CFLAGS = -std=c99 -O1

SRC = main.c \
      utils/utils.c \
      utils/data.c \
      utils/argparse.c \
      model/tree.c \
      model/forest.c \
      eval/eval.c \
      utils/log.c

OBJ = $(SRC:.c=.o)

TARGET = random-forest

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

test: $(TARGET)

	./$(TARGET) data.csv
