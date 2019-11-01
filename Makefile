CXXFLAGS = -fsanitize=address -ggdb3 -O1 -D_GLIBCXX_DEBUG
LDLIBS = -lgmp -lgmpxx

all: de

clean:
	$(RM) de

.PHONY: all clean
