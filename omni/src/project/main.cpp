
#include "common/omStd.h"

#include "volume/omDataVolume.h"
#include "volume/omSegmentation.h"
#include "volume/omVolumeTypes.h"
#include "utility/omImageDataIo.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omProjectData.h"
#include "project/omProject.h"
#include "utility/omHdf5.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;

#include <vtkImageData.h>

#include <hdf5.h>
#include "common/omDebug.h"

void sample_write();

int main(int argc, char *argv[])
{

	switch (argc) {
	case 3:
		cout << argv[1] << " " << argv[2] << endl;
		break;

	default:
		cout << "Usage:" << endl;
		cout << argv[0] << " [directory] [pattern]" << endl;
		return 0;
	}

	string dpath(argv[1]);
	string regex(argv[2]);


	char filename[] = "project.omni";
	OmProject::New("./", filename);


}
