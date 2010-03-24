#include <stdlib.h>
#include "hdf5.h"

int main(int argc, char* argv[])
{
	hid_t dataset,filespace,file;
	void* so;
	void* vo;
	void* vertex;
	float* points;
	uint32_t* stripoffset;
	uint32_t* vertexoffset;


	printf("sizeof(float): %lu \n",sizeof(float));

	file = H5Fopen (argv[1], H5F_ACC_RDWR, H5P_DEFAULT);
	char filepath[100];

	sprintf(filepath,"%s/stripoffset.dat",argv[2]);
	dataset = H5Dopen(file,filepath, H5P_DEFAULT);
	filespace = H5Dget_space (dataset);
	int numberstripoffset = H5Sget_simple_extent_npoints(filespace);
	int size = H5Sget_simple_extent_type(filespace);
	printf("file: %s \n",filepath);
	printf("number: %i   size: %i  \n",numberstripoffset, size);
	H5Sclose(filespace);
        so = (void*) malloc(numberstripoffset*size+1000);
	H5Dread(dataset, H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT,so);

	H5Dclose(dataset);

	//H5Fclose(file);
	//free(stripoffset);

	
	sprintf(filepath,"%s/vertexoffset.dat",argv[2]);
	dataset = H5Dopen(file,filepath, H5P_DEFAULT);
	filespace = H5Dget_space (dataset);
	int numbervertexoffset = H5Sget_simple_extent_npoints(filespace);
	size = H5Sget_simple_extent_type(filespace);
	printf("file: %s \n",filepath);
	printf("number: %i   size: %i  \n",numbervertexoffset, size);
        vo = (void*) malloc(numbervertexoffset*size+1000);
	H5Dread(dataset, H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT,vo);
	H5Sclose(filespace);
	H5Dclose(dataset);

	sprintf(filepath,"%s/vertex.dat",argv[2]);
	dataset = H5Dopen(file,filepath, H5P_DEFAULT);
	filespace = H5Dget_space (dataset);
	int numbervertex = H5Sget_simple_extent_npoints(filespace);
	size = H5Sget_simple_extent_type(filespace);
	printf("file: %s \n",filepath);
	printf("number: %i   size: %i  \n",numbervertex, size);
        vertex = (void *) malloc(numbervertex*size+1000);
	H5Dread(dataset, H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT,vertex);
	H5Sclose(filespace);
	H5Dclose(dataset);
	H5Fclose(file);

	points = (float*) vertex;
	stripoffset = (uint32_t*) so;
	vertexoffset= (uint32_t*) vo;

	FILE* afile;
	
	afile =fopen(argv[3],"w");

	if (afile == NULL){
		printf("Could not open file %s \n",argv[2]);

	} else {
		fprintf(afile,"# vtk DataFile Version 3.0\n");
		fprintf(afile,"vtk output\nASCII\nDATASET POLYDATA\n");
		fprintf(afile,"POINTS %i float\n",(int)(numbervertex/8/3));
		for (int i=0; i<(int)(numbervertex/4);i+=6){
			fprintf(afile,"%f %f %f\n",points[i],points[i+1],points[i+2]);
		}
		
		//uint32_t* strips = (uint32_t*) malloc(numbervertex*size+1000);
		
		size = 0;
		for (int j=0; j<(int)(numberstripoffset/4);j+=2){
			size++;
			for (int k=stripoffset[j]; k < stripoffset[j]+stripoffset[j+1];k++){

				size++;
			}
		}

		fprintf(afile,"\nTRIANGLE_STRIPS %i %i\n",(int)(numberstripoffset/8), size );

		for (int j=0; j<(int)(numberstripoffset/4);j+=2){
			fprintf(afile,"%i ",stripoffset[j+1]);
			for (int k=stripoffset[j]; k < stripoffset[j]+stripoffset[j+1];k++){
				fprintf(afile,"%i ",vertexoffset[k]);
			}
			fprintf(afile,"\n");
		}
	}
	fclose(afile);

}
