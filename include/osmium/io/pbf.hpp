#ifndef OSMIUM_IO_PBF_HPP
#define OSMIUM_IO_PBF_HPP

/*

This file is part of Osmium (http://osmcode.org/osmium).

Copyright 2013 Jochen Topf <jochen@topf.org> and others (see README).

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

#define OSMIUM_LINK_WITH_LIBS_PBF -pthread -lz -lprotobuf-lite -losmpbf

#include <cassert>
#include <stdexcept>

#include <osmpbf/osmpbf.h>

// needed for htonl and ntohl
#ifndef WIN32
# include <netinet/in.h>
#else
# include <winsock2.h>
#endif

#include <osmium/osm/item_type.hpp>

namespace osmium {

    inline item_type osmpbf_membertype_to_item_type(const OSMPBF::Relation::MemberType mt) {
        switch (mt) {
            case OSMPBF::Relation::NODE:
                return item_type::node;
            case OSMPBF::Relation::WAY:
                return item_type::way;
            case OSMPBF::Relation::RELATION:
                return item_type::relation;
            default:
                throw std::runtime_error("Unknown relation member type");
        }
    }

    inline OSMPBF::Relation::MemberType item_type_to_osmpbf_membertype(const item_type type) {
        switch (type) {
            case item_type::node:
                return OSMPBF::Relation::NODE;
            case item_type::way:
                return OSMPBF::Relation::WAY;
            case item_type::relation:
                return OSMPBF::Relation::RELATION;
            default:
                throw std::runtime_error("Unknown relation member type");
        }
    }

} // namespace osmium

#endif // OSMIUM_IO_PBF_HPP
