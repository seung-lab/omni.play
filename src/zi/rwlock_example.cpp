#include "rwlock_server.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

zi::rwlock_server<std::string,int,13> server;

void do_some_acquiering(std::string client)
{
    for ( int i = 0; i < 100; ++i )
    {
        server.acquire_read(client, i%10);
        server.acquire_read(client, i%10);

        std::cout << "Client " << client << " got 2 read locks for ID " << (i%10) << std::endl;
        usleep(50000);

        server.release_read(client, i%10);
        server.release_read(client, i%10);

        std::cout << "Client " << client << " released 2 read locks for ID " << (i%10) << std::endl;

        server.acquire_write(client, i%10);

        std::cout << "Client " << client << " got a write lock for ID " << (i%10) << std::endl;
        usleep(50000);


        server.release_write(client, i%10);

        std::cout << "Client " << client << " released a write lock for ID " << (i%10) << std::endl;
    }
}

int main()
{
    zi::thread t1(zi::bind(&do_some_acquiering,"one_ip_address"));
    zi::thread t2(zi::bind(&do_some_acquiering,"two_ip_address"));

    t1.start();
    t2.start();

    t1.join();
    t2.join();
}
