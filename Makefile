#all:
#	g++ data.cpp  -I./zi_lib -I./zi_lib/external/include -lpthread -lrt

.PHONY:
all:
	g++ -lrt -Wall -ggdb -I../zi_lib -I../zi_lib/external/include main.cpp -o main
.PHONY:
clean:
	echo "Cleaning..."
	rm -rf main

#%.o:%.cpp
#	g++ -lrt -Wall -ggdb -I../zi_lib -I../zi_lib/external/include $< -o $@

#Uncomment to debug Makefile
#OLD_SHELL := $(SHELL)
#SHELL = $(warning [$@ ($^) ($?)]) $(OLD_SHELL)