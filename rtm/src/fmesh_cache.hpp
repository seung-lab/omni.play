//
// Copyright (C) 2012  Aleksandar Zlateski <zlateski@mit.edu>
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
#include "detail/garbage.hpp"

#include <zi/utility/non_copyable.hpp>
#include <zi/concurrency.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>


namespace zi {
namespace mesh {

class fmesh_cache: non_copyable
{
private:
    struct entry
    {
        vec4u                 id_  ;
        mutable mesh_type_ptr mesh_;

        entry(const vec4u& id, const mesh_type_ptr& mesh)
            : id_(id)
            , mesh_(mesh)
        { }

        void update( const mesh_type_ptr& mesh ) const
        {
            // make sure the memory freeing is done in a different thread
            garbage.add(mesh_);
            mesh_ = mesh;
        }
    };

    typedef boost::multi_index_container<
        entry,
        boost::multi_index::indexed_by<
            boost::multi_index::sequenced<>
            , boost::multi_index::ordered_unique<
                  BOOST_MULTI_INDEX_MEMBER(entry,vec4u,id_)
                  >
            >
        > item_list;

    typedef item_list::iterator iterator;

    item_list   list_    ;
    zi::mutex   mutex_   ;
    std::size_t max_size_;
    std::size_t size_    ;

    void insert( const entry& item )
    {
        std::pair< iterator, bool > p = list_.push_front(item);

        size_ += item.mesh_ ? item.mesh_->mem_size() : 0;

        if( !p.second )
        {
            list_.relocate(list_.begin(), p.first);
            size_ -= p.first->mesh_ ? p.first->mesh_->mem_size() : 0;
            p.first->update(item.mesh_);
        }
        else
        {
            while ( size_ > max_size_ )
            {
                size_ -= list_.back().mesh_ ? list_.back().mesh_->mem_size() : 0;
                garbage.add(list_.back().mesh_);
                list_.pop_back();
            }
        }
    }

public:

    fmesh_cache( std::size_t s )
        : list_()
        , mutex_()
        , max_size_( s * 1024 * 1024 )
        , size_(0)

    void insert( const vec4u& k, const mesh_type_ptr& v )
    {
        zi::mutex::guard g(mutex_);
        insert( entry(k,v) );
    }

    mesh_type_ptr get( const vec4u& k )
    {
        zi::mutex::guard g(mutex_);
        typedef item_list::nth_index<1>::type key_index;

        key_index& index = list_.get<1>();

        key_index::iterator it = index.find(k);

        if ( it != index.end() )
        {
            return it->mesh_;
        }

        insert( entry(k, mesh_type_ptr() ) );

        return mesh_type_ptr();
    }


}; // class fmesh_cache


} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_FMESH_IO_HPP
