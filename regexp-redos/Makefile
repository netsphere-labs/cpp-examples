
TARGETS = icu libc1 pcre re2 boost re2-sub
all: $(TARGETS)

CXXFLAGS = -Wall -Wextra

icu: icu.o
	$(LINK.o) -lstdc++ -licui18n -licuuc -o $@ $^

libc1: libc1.o
	$(LINK.o) -lstdc++ -o $@ $^

pcre: pcre.o
	$(LINK.o) -lstdc++ -lpcre2-8 -o $@ $^

re2: re2.o
	$(LINK.o) -lstdc++ -lre2 -o $@ $^

re2-sub: re2-sub.o
	$(LINK.o) -lstdc++ -lre2 -o $@ $^

boost: boost.o
	$(LINK.o) -lstdc++ -lboost_regex -licui18n -licuuc -o $@ $^

clean:
	rm -f $(TARGETS) *.o a.out
