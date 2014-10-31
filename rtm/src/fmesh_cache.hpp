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
    std::size_t    max_memory_;
    std::size_t    cur_memory_;

    std::map<vec5u, mesh_type_ptr> map_;

private:
    void try_evict_nl()
    {
        if ( lru_.size() > size_ )
        {
            std::size_t x = size_ * 3 / 4;
            while ( lru_.size() > x )
            {
                vec5u to_erase = lru_.pop_lr_used();
                cur_memory_ -= map_[to_erase]->mem_size();
                map_.erase(to_erase);
            }
        }

        if ( max_memory_ < cur_memory_ )
        {
            std::size_t target_memory = max_memory_ * 3 / 4;
            while ( cur_memory_ > target_memory )
            {
                vec5u to_erase = lru_.pop_lr_used();
                cur_memory_ -= map_[to_erase]->mem_size();
                map_.erase(to_erase);
            }
        }
    }

public:
    fmesh_cache_impl()
        : lru_()
        , m_()
        , size_(500000)
        , max_memory_(10000000000ULL) // 10G
        , cur_memory_(0)
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

        if ( map_.count(id) )
        {
            cur_memory_ -= map_[id]->mem_size();
        }

        cur_memory_ += m->mem_size();

        map_[id] = m;
        lru_.poke(id);

        try_evict_nl();
    }

    void remove(uint32_t mip, const vec4u& c)
    {
        vec5u id(c, mip);
        zi::mutex::guard g(m_);
        if ( map_.count(id) )
        {
            cur_memory_ -= map_[id]->mem_size();
        }
        lru_.erase(id);
        map_.erase(id);
    }

    void clear()
    {
        zi::mutex::guard g(m_);
        cur_memory_ = 0;
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
