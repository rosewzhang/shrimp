CXXC:=clang++
CXXFLAGS := -std=c++20 

SRCS:=$(wildcard *.cpp)
OBJS:=$(SRCS:.cpp=.o)

all: a.out

run: a.out
	./a.out token.txt bot.txt bot_in.txt

run_reset: a.out
	./a.out token.txt bot.txt

a.out: $(OBJS)
	$(CXXC) $(CXXFLAGS) -ldpp $(OBJS) -o a.out	

%.o: %.cpp
	$(CXXC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -fv a.out *.o

