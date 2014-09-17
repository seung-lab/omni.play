//
// Copyright (C) 2012-2014  Aleksandar Zlateski <zlateski@mit.edu>
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

#ifndef ZI_MESHING_FMESH_CACHE_HPP
#define ZI_MESHING_FMESH_CACHE_HPP

#include "types.hpp"

#include <zi/utility/non_copyable.hpp>
#include <zi/concurrency.hpp>
#include <zi/utility/singleton.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>

#include <map>

#include "detail/lru.hpp"

namespace zi {
namespace mesh {

class fmesh_cache_impl: non_copyable
{
private:
    zi::lru<vec5u> lru_ ;
    zi::mutex      m_   ;
    std::size_t    size_;

    std::map<vec5u, mesh_type_ptr> map_;

private:
    void try_evict_nl()
    {
        if ( lru_.size() > size_ )
        {
            std::size_t x = size_ * 3 / 4;
            while ( lru_.size() > x )
            {
                map_.erase(lru_.pop_lr_used());
            }
        }
    }

public:
    fmesh_cache_impl()
        : lru_()
        , m_()
        , size_(5000)
        , map_()
    {}

    mesh_type_ptr get(uint32_t mip, const vec4u& c)
    {
        mesh_type_ptr ret;
        vec5u id(c, mip);
        zi::mutex::guard g(m_);
        if ( map_.count(id) )
        {
            ret = map_[id];
        }

        return ret;
    }

    void set(uint32_t mip, const vec4u& c, const mesh_type_ptr& m)
    {
        vec5u id(c, mip);
        zi::mutex::guard g(m_);

        map_[id] = m;
        lru_.poke(id);

        try_evict_nl();
    }

    void remove(uint32_t mip, const vec4u& c)
    {
        vec5u id(c, mip);
        zi::mutex::guard g(m_);
        lru_.erase(id);
        map_.erase(id);
    }

    void clear()
    {
        zi::mutex::guard g(m_);
        map_.clear();
        lru_.clear();
    }

}; // class fmesh_cache


namespace {
fmesh_cache_impl& fmesh_cache = zi::singleton<fmesh_cache_impl>::instance();
}

} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_FMESH_CACHE_HPP
