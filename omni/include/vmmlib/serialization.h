
/*
 *	Non-obtrusive serialization for use with the boost::serialization library.
 *	
 *	Not part of the original VMML library.  Added for use in the Omni project.
 *
 *	Brett Warne - bwarne@mit.edu - 3/10/2009.
 */


#ifndef VMML_SERIALIZATION_H
#define VMML_SERIALIZATION_H

#include "vector3.h"
#include "axisAlignedBoundingBox.h"

//split serialization
#include <boost/serialization/split_free.hpp>


namespace boost { 
	namespace serialization {
		
		//Vector3<T>
		template<class Archive, class T>
		inline void 
		serialize(Archive & ar, vmml::Vector3<T> & v, const unsigned int file_version) {
			ar & v.array;
		}
		
		//Vector4<T>
		template<class Archive, class T>
		inline void 
		serialize(Archive & ar, vmml::Vector4<T> & v, const unsigned int file_version) {
			ar & v.array;
		}
		
		//Matrix4<T>
		template<class Archive, class T>
		inline void 
		serialize(Archive & ar, vmml::Matrix4<T> & m, const unsigned int file_version) {
			ar & m.array;
		}
		
		
		/* 
		 * AxisAlignedBoundingBox<T>
		 *
		 * obfuscated split serialization so as not to break into protected members 
		 */
		template<class Archive, class T>
		inline void 
		save(Archive & ar, const vmml::AxisAlignedBoundingBox<T> & aabb, const unsigned int file_version) {
			vmml::Vector3<T> min = aabb.getMin();
			vmml::Vector3<T> max = aabb.getMax();
			bool dirty = aabb.isDirty();
			bool empty = aabb.isEmpty();
			
			ar & min;
			ar & max;
			ar & dirty;
			ar & empty;
		}
		template<class Archive, class T>
		inline void 
		load(Archive & ar, vmml::AxisAlignedBoundingBox<T> & aabb, const unsigned int file_version) {
			vmml::Vector3<T> min, max;
			bool dirty, empty;
			
			ar & min;
			ar & max;
			ar & dirty;
			ar & empty;
			
			vmml::AxisAlignedBoundingBox<T> temp(min, max);
			aabb.setDirty(dirty);
			aabb.setEmpty(empty);
			aabb = temp;
		}
		template<class Archive, class T>
		inline void 
		serialize(Archive & ar, vmml::AxisAlignedBoundingBox<T> & aabb, const unsigned int file_version) {
			boost::serialization::split_free(ar, aabb, file_version);
		}
		
	} // serialization
} // namespace boost





#endif
