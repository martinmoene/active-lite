// main-queue.cpp

#include "nonstd/active.hpp"
#include <iostream>

int main()
{
    nonstd::active::queue<int> aq;

    // disallow copying of queue:
    // auto other{ aq };

    auto send = [&]()
    {
        for ( auto x : {1, 2, 3} )
        {
            aq.send( x );
            std::cout << "s" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    };

    auto recv = [&]()
    {
        for ( auto x : {1, 2, 3} )
        {
            std::cout << aq.receive() << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    std::thread ts( send );
    std::thread tr( recv );
    ts.join();
    tr.join();
}

// cl -EHsc -I../include main-queue.cpp && main-queue.exe
// clang-cl -Wall -Wextra -Wno-c++98-compat -EHsc -I../include main-queue.cpp && main-queue.exe
