#ifndef OM_RAND_COLOR_FILE_HPP
#define OM_RAND_COLOR_FILE_HPP

#include "common/omCommon.h"
#include "utility/omRand.hpp"
#include "datalayer/fs/omFileNames.hpp"

class OmRandColorFile {
private:
	static const int version_ = 1;

	const QString fnp_;

	boost::shared_ptr<QFile> file_;
	OmColor* values_;
	int64_t numEntries_;

	friend class OmProjectGlobals;

public:
	OmRandColorFile()
		: fnp_(fileName())
		, values_(NULL)
		, numEntries_(0)
	{}

	~OmRandColorFile()
	{}

	OmColor GetRandomColor()
	{
		assert(values_);
		const int index = OmRand::GetRandomInt(0, numEntries_-1);
        return values_[index];
    }

private:
	void createOrLoad()
	{
		if(!QFile::exists(fnp_)){
			setupFile();
		}
		mapReadOnly();
	}

	void mapReadOnly()
	{
		file_ = boost::make_shared<QFile>(fnp_);
		if(!file_->exists()){
			throw OmIoException("file doesn't exist", fnp_);
		}

		if(!file_->open(QIODevice::ReadOnly)) {
			throw OmIoException("could not open", fnp_);
		}

		uchar* map = file_->map(0, file_->size());
		if(!map){
			throw OmIoException("could not map", fnp_);
		}

		file_->close();

		values_ = reinterpret_cast<OmColor*>(map);
		numEntries_ = file_->size() / sizeof(OmColor);
	}

	QString fileName()
	{
		return OmFileNames::GetRandColorFileName()
			+ ".ver" + QString::number(version_);
	}

	static void buildColorTable(std::vector<OmColor>& colorTable)
	{
		// make sure to change version_ if change color table algorithm...

		static const int min_variance = 120;

		colorTable.clear();
		colorTable.reserve(1952449); // 1,952,448 entries for min_var = 120

		for(int r = 0; r < 128; ++r){
			for(int g = 0; g < 128; ++g){
				for(int b = 0; b < 128; ++b){

					const int avg  = ( r + g + b ) / 3;
					const int avg2 = ( r*r + g*g + b*b ) / 3;
					const int v = avg2 - avg*avg;

					if(v >= min_variance){
						const OmColor color = {r, g, b};
						colorTable.push_back(color);
					}
				}
			}
		}
	}

	void setupFile()
	{
		std::vector<OmColor> colorTable;

		buildColorTable(colorTable);

		QFile file(fnp_);
		if(!file.open(QIODevice::ReadWrite)) {
			throw OmIoException("could not open", fnp_);
		}

		const int64_t numBytes = colorTable.size() * sizeof(OmColor);
		file.resize(numBytes);

		const char* data = reinterpret_cast<const char*>(&colorTable[0]);
		file.write(data, numBytes);
	}
};

#endif
