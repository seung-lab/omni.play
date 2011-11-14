#all:
#	g++ data.cpp  -I./zi_lib -I./zi_lib/external/include -lpthread -lrt

.PHONY:
all:data.o mapped_data.o

.PHONY:
clean:
	echo "Cleaning..."
	rm -rf *.o

%.o:%.cpp
	g++ -lrt -Wall -ggdb -I../zi_lib -I../zi_lib/external/include $< -o $@ 

#Uncomment to debug Makefile
#OLD_SHELL := $(SHELL)
#SHELL = $(warning [$@ ($^) ($?)]) $(OLD_SHELL)