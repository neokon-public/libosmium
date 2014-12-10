#ifndef OSMIUM_IO_DETAIL_READ_THREAD_HPP
#define OSMIUM_IO_DETAIL_READ_THREAD_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013,2014 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <atomic>
#include <chrono>
#include <ratio>
#include <string>
#include <thread>
#include <utility>

#include <osmium/io/compression.hpp>
#include <osmium/thread/queue.hpp>
#include <osmium/thread/util.hpp>

namespace osmium {

    namespace io {

        namespace detail {

            class ReadThread {

                osmium::thread::Queue<std::string>& m_queue;
                osmium::io::Decompressor* m_decompressor;

                // If this is set in the main thread, we have to wrap up at the
                // next possible moment.
                std::atomic<bool>& m_done;

            public:

                explicit ReadThread(osmium::thread::Queue<std::string>& queue, osmium::io::Decompressor* decompressor, std::atomic<bool>& done) :
                    m_queue(queue),
                    m_decompressor(decompressor),
                    m_done(done) {
                }

                bool operator()() {
                    osmium::thread::set_thread_name("_osmium_input");

                    try {
                        while (!m_done) {
                            std::string data {m_decompressor->read()};
                            if (data.empty()) {
                                m_queue.push(std::move(data));
                                break;
                            }
                            m_queue.push(std::move(data));
                            while (m_queue.size() > 10 && !m_done) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                            }
                        }

                        m_decompressor->close();
                    } catch (...) {
                        // If there is an exception in this thread, we make sure
                        // to push an empty string onto the queue to signal the
                        // end-of-data to the reading thread so that it will not
                        // hang. Then we re-throw the exception.
                        m_queue.push(std::string());
                        throw;
                    }
                    return true;
                }

            }; // class ReadThread

        } // namespace detail

    } // namespace io

} // namespace osmium

#endif // OSMIUM_IO_DETAIL_READ_THREAD_HPP
