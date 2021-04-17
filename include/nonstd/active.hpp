//
// Copyright (c) 2020-2021 Martin Moene
//
// https://github.com/martinmoene/boolean-lite
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// active-lite is inspired on:
// Prefer Using Active Objects Instead of Naked Threads, Herb Sutter
// http://drdobbs.com/go-parallel/article/225700095
// https://www.state-machine.com/doc/Sutter2010a.pdf

#ifndef NONSTD_ACTIVE_LITE_HPP
#define NONSTD_ACTIVE_LITE_HPP

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#define active_DISABLE_COPY( X )            \
    private:                                \
        X( X && ) = delete;                 \
        X( const X & ) = delete;            \
        X & operator=( X && ) = delete;     \
        X & operator=( const X & ) = delete;

namespace nonstd {
namespace active {

// A thread-save queue.

template< typename T >
class queue
{
    active_DISABLE_COPY( queue )

public:
    queue() = default;

    // put an item in the queue:

    void send( T const & item )
    {
        // std::scoped_lock lock( m_mutex );
        std::unique_lock<std::mutex> lock( m_mutex );
        m_queue.push( item );
        m_item_available.notify_one();
    }

    // get an item from the queue, waiting if required:

    T receive()
    {
        // std::scoped_lock lock( m_mutex );
        std::unique_lock<std::mutex> lock( m_mutex );

        if ( m_queue.empty() )
        {
            m_item_available.wait( lock, [this]{ return !this->m_queue.empty(); } );
        }

        T item{ m_queue.front() };
        m_queue.pop();

        return item;
    }

private:
    // the non-monitored queue:
    std::queue<T> m_queue;

    // the queue access mutex:
    std::mutex m_mutex;

    // the item-available condition:
    std::condition_variable m_item_available;
};

// Helper class for Active Object pattern,
// see http://en.wikipedia.org/wiki/Active_object.

class helper
{
    active_DISABLE_COPY( helper )

public:
    // base of all message types:

    class message
    {
    public:
        virtual ~message()
        {}

        virtual void execute()
        {
            assert( 0 && "Override nonstd::active::helper::message::execute()" );
        }
    };

    // start everything up, using Run as the thread mainline:

    helper()
    : m_done  ( new message )
    , m_thread( new std::thread( &helper::run, this ) )
    {}

    // shut down: send sentinel and wait for queue to drain:

    ~helper()
    {
        send( m_done );
        m_thread->join();
    }

    // enqueue a message:

    void send( std::shared_ptr<message> msg )
    {
        m_queue.send( msg );
    }

private:
    // the dispatch loop: pump messages until done:

    void run()
    {
        std::shared_ptr<message> msg;
        while ( ( msg = m_queue.receive() ) != m_done )
        {
            msg->execute();
        }
    }

private:
    // sentinel, end of messages, stop running:
    std::shared_ptr<message> m_done;

    // the message queue:
    queue< std::shared_ptr<message> > m_queue;

    // this thread:
    std::unique_ptr<std::thread> m_thread;
};

} // namespace active
} // namespace nonstd

#endif // NONSTD_ACTIVE_LITE_HPP
