# Declaration of variables
CXX = gcc
CC_FLAGS = -w

# File names
EXEC = minisearch
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC) -lm

# To obtain object files
%.o: %.cpp
	$(CXX) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
