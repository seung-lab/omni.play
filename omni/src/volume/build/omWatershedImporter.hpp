#ifndef OM_WATERSHED_IMPORTER_HPP
#define OM_WATERSHED_IMPORTER_HPP

#include "common/omCommon.h"
#include "utility/omTimer.h"
#include "volume/omVolumeData.hpp"
#include "volume/omVolumeTypes.hpp"
#include "volume/omSegmentation.h"

class OmWatershedImporter {
private:
	OmSegmentation& vol_;
	const QString fnp_;

public:
	OmWatershedImporter(OmSegmentation& vol, const QString& fnp)
		: vol_(vol)
		, fnp_(fnp)
	{}

	void Import()
	{
		OmTimer timer;

		printf("\timporting data...\n");

		doImport();

		timer.PrintDone();
	}

private:
	void doImport()
	{
		vol_.SetBuildState(MIPVOL_BUILDING);

		vol_.SetDataDimensions(getMip0Dims());
		vol_.UpdateRootLevel();

		checkDims();
		moveFiles();

		findSegmentBoundingBoxes();

		vol_.SetBuildState(MIPVOL_BUILT);
	}

	Vector3i getMip0Dims()
	{
		assert(0 && "fixme");
		return Vector3i();
	}

	void checkDims()
	{
		assert(0 && "fixme");
	}

	void moveFiles()
	{
		assert(0 && "fixme");
	}

	void findSegmentBoundingBoxes()
	{
		assert(0 && "fixme");
	}
};

#endif
