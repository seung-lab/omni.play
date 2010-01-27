#ifndef OM_VTK_H
#define OM_VTK_H

/*
 *  Header for Omni to vtk conversions.
 *
 *  Brett Warne - bwarne@mit.edu - 3/8/09
 */


#include <vtkType.h>


inline
int
bytesToVtkScalarType(int bytes) {
	
	switch(bytes) {
		case 1:
			return VTK_UNSIGNED_CHAR;
		case 4:
			return VTK_UNSIGNED_INT;
		default:
			assert(false);
	}
}



inline
int
vtkScalarTypeToBytes(int vtkScalarType) {
	
	switch(vtkScalarType) {
		case VTK_UNSIGNED_CHAR:
			return 1;
		case VTK_UNSIGNED_INT:
			return 4;
		default:
			assert(false);
	}
}


#endif