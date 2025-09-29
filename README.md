# active lite: Active objects for C++11 and later

[![License](https://img.shields.io/badge/license-BSL-blue.svg)](https://opensource.org/licenses/BSL-1.0) [![Build Status](https://github.com/martinmoene/active-lite/actions/workflows/ci.yml/badge.svg)](https://github.com/martinmoene/active-lite/actions/workflows/ci.yml)

The code of _active-lite_ is inspired on the 2010 DDJ article [_Prefer Using Active Objects Instead of Naked Threads_](https://herbsutter.com/2010/07/12/effective-concurrency-prefer-using-active-objects-instead-of-naked-threads/) by Herb Sutter, with subtitle _How to automate best practices for threads and raise the semantic level of our code_.

A [PDF of the article](./doc/Sutter2010a.pdf) is present in the _doc_ folder.

See also [Active Object pattern](http://en.wikipedia.org/wiki/Active_object) on Wikipedia.

## Example usage

Create a counter task that counts from 0 to 10, one step per 100 ms.

```C++
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
```

### Compile and run

```Cmd
prompt> g++ -Wall -I../include main-count.cpp -o main-count.exe && main-count.exe
++main line begin
--main line end
Counter: 0
Counter: 1
Counter: 2
Counter: 3
Counter: 4
Counter: 5
Counter: 6
Counter: 7
Counter: 8
Counter: 9
```

### Synopsis

#### Types in namespace nonstd::active

| Type             | Notes                                  |
| ---------------- | -------------------------------------- |
| class **queue**  | Thread-safe queue                      |
| class **helper** | Helper class for Active Object pattern |
