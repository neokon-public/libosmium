#ifndef OSMIUM_INDEX_MULTIMAP_SPARSE_MEM_MULTIMAP_HPP
#define OSMIUM_INDEX_MULTIMAP_SPARSE_MEM_MULTIMAP_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013-2015 Jochen Topf <jochen@topf.org> and others (see README).

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

#include <algorithm>
#include <cstddef>
#include <map>
#include <utility>
#include <vector>

#include <osmium/index/multimap.hpp>
#include <osmium/io/detail/read_write.hpp>

namespace osmium {

    namespace index {

        namespace multimap {

            /**
             * This implementation uses std::multimap internally. It uses rather a
             * lot of memory, but might make sense for small maps.
             */
            template <typename TId, typename TValue>
            class SparseMemMultimap : public osmium::index::multimap::Multimap<TId, TValue> {

                // This is a rough estimate for the memory needed for each
                // element in the map (id + value + pointers to left, right,
                // and parent plus some overhead for color of red-black-tree
                // or similar).
                static constexpr size_t element_size = sizeof(TId) + sizeof(TValue) + sizeof(void*) * 4;

            public:

                typedef typename std::multimap<const TId, TValue> collection_type;
                typedef typename collection_type::iterator iterator;
                typedef typename collection_type::const_iterator const_iterator;
                typedef typename collection_type::value_type value_type;

                typedef typename std::pair<TId, TValue> element_type;

            private:

                collection_type m_elements;

            public:

                SparseMemMultimap() = default;

                ~SparseMemMultimap() noexcept final = default;

                void unsorted_set(const TId id, const TValue value) {
                    m_elements.emplace(id, value);
                }

                void set(const TId id, const TValue value) final {
                    m_elements.emplace(id, value);
                }

                std::pair<iterator, iterator> get_all(const TId id) {
                    return m_elements.equal_range(id);
                }

                std::pair<const_iterator, const_iterator> get_all(const TId id) const {
                    return m_elements.equal_range(id);
                }

                void remove(const TId id, const TValue value) {
                    std::pair<iterator, iterator> r = get_all(id);
                    for (iterator it = r.first; it != r.second; ++it) {
                        if (it->second == value) {
                            m_elements.erase(it);
                            return;
                        }
                    }
                }

                iterator begin() {
                    return m_elements.begin();
                }

                iterator end() {
                    return m_elements.end();
                }

                size_t size() const final {
                    return m_elements.size();
                }

                size_t used_memory() const final {
                    return element_size * m_elements.size();
                }

                void clear() final {
                    m_elements.clear();
                }

                void consolidate() {
                    // intentionally left blank
                }

                void dump_as_list(const int fd) final {
                    std::vector<element_type> v;
                    v.reserve(m_elements.size());
                    for (const auto& element : m_elements) {
                        v.emplace_back(element.first, element.second);
                    }
                    std::sort(v.begin(), v.end());
                    osmium::io::detail::reliable_write(fd, reinterpret_cast<const char*>(v.data()), sizeof(element_type) * v.size());
                }

            }; // class SparseMemMultimap

        } // namespace multimap

    } // namespace index

} // namespace osmium

#endif // OSMIUM_INDEX_MULTIMAP_SPARSE_MEM_MULTIMAP_HPP
