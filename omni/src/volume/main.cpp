
#include "common/omStd.h"

#include "system/omGenericManager.h"
#include "system/omProject.h"

#include "omMipVolume.h"

#include "omDataVolume.h"
#include "omMipVolume.h"
#include "omVolume.h"

#include <fstream>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs = boost::filesystem;

#include <QApplication>
#include "common/omDebug.h"

void build_mip_volume(string dpath, string regex);

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	switch (argc) {
	case 3:
		cout << argv[1] << " " << argv[2] << endl;
		break;

	default:
		cout << argc << endl;
		cout << "Usage:" << endl;
		cout << argv[0] << " [directory] [regex]" << endl;
		return 0;
	}

	string dpath(argv[1]);
	string regex(argv[2]);

	build_mip_volume(dpath, regex);
}

void build_mip_volume(string dpath, string regex)
{

	OmProject::New("./", "project.omni");

	OmMipVolume & r_channel = OmVolume::AddChannel();
	r_channel.SetSourceDirectoryPath(dpath);
	r_channel.SetSourceFilenameRegex(regex);

	cout << "source files found: " << r_channel.GetSourceFilenameRegexMatches().size() << endl;

	r_channel.Build();

	OmProject::Save();
	OmProject::Close();
}

/*
int main(int argc, char* argv[])
{
	switch(argc) {
		case 4:
			cout << argv[1] << argv[2] << endl;
			cout << argv[3] << endl;
			break;

		default:
			cout << "Usage:" << endl;
			cout << argv[0] << "[directory] [filename] [sourcepath] " << endl;
			return 0;
	}
	
	string dpath(argv[1]);
	string fname(argv[2]);
	string spath(argv[3]);
	
	OmProject::New(dpath, fname);
	
	int size;
	time_t start_time, end_time;
	
	OmSegmentation &r_segmentation = OmVolume::AddSegmentation();
	r_segmentation.SetSourceDirectoryPath(spath);
	
	fprintf(stderr, "--------------------------------------\n");
	cout << "segmentation: " << spath << endl;
	fprintf(stderr, "segmentation: %s\n", spath.c_str());
	cout << endl;
	cout << "build segmentation volume" << endl;
	
	time(&start_time);
	r_segmentation.BuildVolumeData();
	time(&end_time);
	fprintf(stderr, "elapsed volume build time: %f\n", difftime(end_time, start_time));
	
	OmProject::Save();
	size = bfs::file_size(bfs::path(dpath+fname));	
	fprintf(stderr, "project size: %d\n", size);
	
	
	
	cout << endl;
	cout << "build segmentation mesh" << endl;
	
	time(&start_time);
	r_segmentation.BuildMeshData();
	time(&end_time);
	fprintf(stderr, "elapsed mesh build time: %f\n", difftime(end_time, start_time));
	
	OmProject::Save();
	size = bfs::file_size(bfs::path(dpath+fname));	
	fprintf(stderr, "project size: %d\n\n\n", size);
	
}
*/
