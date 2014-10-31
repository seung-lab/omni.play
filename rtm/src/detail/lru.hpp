#pragma once
//
// Copyright (C) 2014  Aleksandar Zlateski <zlateski@mit.edu>
// ----------------------------------------------------------
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <cstddef>

namespace zi {

template < typename T >
class lru
{
private:
    typedef boost::multi_index::multi_index_container<
        T,
        boost::multi_index::indexed_by<
            boost::multi_index::sequenced<>,
            boost::multi_index::ordered_unique< boost::multi_index::identity<T> >
        >
    >
    container_type;

    typedef typename container_type::iterator iterator;

    typedef typename boost::multi_index::nth_index<container_type,0>::type seq_index;
    typedef typename boost::multi_index::nth_index<container_type,1>::type ord_index;

private:
    container_type container;

public:
    std::size_t size() const
    {
        return container.size();
    }

    void poke( const T& v )
    {
        std::pair<iterator,bool> p = container.push_front(v);

        if ( !p.second )
        {
            container.relocate(container.begin(), p.first);
        }
    }

    T pop_lr_used()
    {
        T ret = container.back();
        container.pop_back();
        return ret;
    }

    void erase(const T& v)
    {
        ord_index& index = container.template get<1>();
        index.erase(v);
    }

    void clear()
    {
        container.clear();
    }

}; // class lru

} // namespace zi

