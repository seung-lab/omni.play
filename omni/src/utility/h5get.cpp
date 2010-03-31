#include <stdlib.h>
#include "hdf5.h"

int main(int argc, char* argv[])
{
	if (argc!=3){
		printf("Wrong Number of Arguments!!!");
		return 0;
	}
	hid_t dataset,filespace,file;
	void* data;
	file = H5Fopen (argv[1], H5F_ACC_RDWR, H5P_DEFAULT);
	dataset = H5Dopen(file,argv[2], H5P_DEFAULT);
	filespace = H5Dget_space (dataset);
	int number = H5Sget_simple_extent_npoints(filespace);
	int size = H5Sget_simple_extent_type(filespace);
	if (size == sizeof(uint32_t)){
		data = (uint32_t*) malloc(number*size);
		H5Dread(dataset, H5T_NATIVE_UINT32, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
	} else if (size == sizeof(char)){
		data = (char*) malloc(number*size);
		H5Dread(dataset, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);		
	} else {
		printf("Data Element Size = %i \n",size);
	}

	FILE* afile;
	afile =fopen(argv[3],"wb");
	if (afile != NULL){
		fwrite(data,number,size,afile);
	} else {
		printf("Could not open file %s \n",argv[2]);
	}
	fclose(afile);
}
