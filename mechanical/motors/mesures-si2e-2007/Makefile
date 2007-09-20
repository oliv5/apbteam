FILES = a b c d e f g h i j

all: $(FILES)

%: %.raw filter
	./filter < $< > $@

filter: filter.c

clean:
	rm -f filter $(FILES)
