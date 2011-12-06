 #all:
#	g++ data.cpp  -I./zi_lib -I./zi_lib/external/include -lpthread -lrt

.PHONY:
all:
	#g++ -lrt -Wall -ggdb -I/home/zlateski/code/zi_lib -I/home/zlateski/code/zi_lib/external/include main.cpp -o main -lpthread -lrt
	g++ -lrt -Wall -ggdb -I/usr/local/zi_lib -I/usr/local/zi_lib/external/include main.cpp -o main -lpthread -lrt
.PHONY:
clean:
	echo "Cleaning..."
	rm -rf main

#%.o:%.cpp
#	g++ -lrt -Wall -ggdb -I../zi_lib -I../zi_lib/external/include $< -o $@

#Uncomment to debug Makefile
#OLD_SHELL := $(SHELL)
#SHELL = $(warning [$@ ($^) ($?)]) $(OLD_SHELL)