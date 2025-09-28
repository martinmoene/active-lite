// main-count.cpp

#include "nonstd/active.hpp"
#include <iostream>

/**
 * backgrounder.
 */
class counter
{
    using message = nonstd::active::helper::message;
    using base_shared_ptr = std::shared_ptr< message >;

public:
    // send the count request to the background worker class:

    void count( int const to )
    {
        helper.send( base_shared_ptr( new worker( this, to ) ) );
    }

    // the background worker class:

    class worker: public message
    {
    private:
        const int m_to;

    public:
        worker( counter * /*p*/, int to )
        : m_to( to )
        {}

        void execute() override
        {
            for ( int i = 0; i < m_to; ++i )
            {
                std::cout << "Counter: " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    };

    // Active helper goes last, for ordered destruction.

    nonstd::active::helper helper;
};

int main()
{
    std::cout << "++main line begin" << std::endl;

    counter c; c.count( 10 );

    std::cout << "--main line end" << std::endl;
}

// cl -EHsc -I../include main-count.cpp && main-count.exe
// clang-cl -Wall -Wextra -Wno-c++98-compat -EHsc -I../include main-count.cpp && main-count.exe
// g++ -Wall -Wextra -Wno-c++98-compat -I../include main-count.cpp -o main-count.exe && main-count.exe

// Output:
//
// ++main line begin
// --main line end
// Counter: 0
// Counter: 1
// Counter: 2
// Counter: 3
// Counter: 4
// Counter: 5
// Counter: 6
// Counter: 7
// Counter: 8
// Counter: 9
