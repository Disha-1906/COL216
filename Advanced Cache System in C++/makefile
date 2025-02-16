CC = g++
# Executable name
EXEC = cache_simulate

# Default rule
all: $(EXEC)

$(EXEC): cache.cpp
	$(CC) cache.cpp -o $(EXEC)

# Clean rule
clean:
	rm -f $(EXEC)