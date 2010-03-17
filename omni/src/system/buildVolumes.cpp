#include "buildVolumes.h"

BuildVolumes::BuildVolumes()
{
}

void BuildVolumes::addFileNameAndPath( QString fnp )
{
	mFileNamesAndPaths << fnp;
}

void BuildVolumes::setFileNamesAndPaths( QFileInfoList fileNamesAndPaths )
{
	mFileNamesAndPaths = fileNamesAndPaths;
}

void BuildVolumes::buildAndMeshSegmentation( OmSegmentation * current_seg )
{
	build_seg_image(current_seg);
	build_seg_mesh(current_seg);
}

void BuildVolumes::build_seg_image(OmSegmentation * current_seg)
{
	printf("starting segmentation data build...\n");
	time_t start;
	time_t end;
	double dif;
	
	time (&start);
	current_seg->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
	current_seg->BuildVolumeData();
	time (&end);
	dif = difftime(end, start);
	printf("segmentation data build performed in (%.2lf secs)\n", dif );
}

void BuildVolumes::build_seg_mesh(OmSegmentation * current_seg)
{
	printf("starting segmentation mesh build...\n");
	time_t start;
	time_t end;
	double dif;
	
	time (&start);
	current_seg->BuildMeshData();
	time (&end);
	dif = difftime(end, start);
	printf("segmentation mesh build performed in (%.2lf secs)\n", dif );
}

void BuildVolumes::build_channel(OmChannel * current_channel)
{
	printf("starting channel build...\n");
	time_t start;
	time_t end;
	double dif;
	
	time (&start);
	current_channel->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
	current_channel->BuildVolumeData();
	time (&end);
	dif = difftime(end, start);
	printf("channel build performed in (%.2lf secs)\n", dif );
}

	
