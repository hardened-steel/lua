#include <boost/range/adaptor/filtered.hpp>
#include <boost/fiber/all.hpp>
#include <boost/assign.hpp>
#include <iterator>
#include <iostream>
#include <vector>

typedef boost::fibers::buffered_channel< int > channel_t;

void send( channel_t & chan) {
    for ( int i = 0; i < 5; ++i) {
        chan.push( i);
    }
    chan.close();
}

void recv( channel_t & chan) {
    int i;
    while ( boost::fibers::channel_op_status::success == chan.pop(i) ) {
        std::cout << "received " << i << std::endl;
    }
}

int main(int argc, const char* argv[])
{
	channel_t chan{ 2 };
	boost::fibers::fiber f1( std::bind( send, std::ref( chan) ) );
	boost::fibers::fiber f2( std::bind( recv, std::ref( chan) ) );

	f1.join();
	f2.join();
}