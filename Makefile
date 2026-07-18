CC = gcc
CFLAGS = -O3 -fopenmp -Wall

GEN = generate_random_data
SORT = bucket_sort
CHECK = check_sorted_data

all: run

$(GEN): generate_random_data.c
	$(CC) $(CFLAGS) -o $(GEN) generate_random_data.c 1000000

$(SORT): main.c
	$(CC) $(CFLAGS) -o $(SORT) main.c

$(CHECK): check_sorted_data.c
	$(CC) $(CFLAGS) -o $(CHECK) check_sorted_data.c

run: $(GEN) $(SORT) $(CHECK)
	./$(GEN) 1000000
	./$(SORT)
	./$(CHECK)

clean:
	rm -f $(GEN) $(SORT) $(CHECK)