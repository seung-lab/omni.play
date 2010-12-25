#ifndef OM_HDF5_MANAGER_H
#define OM_HDF5_MANAGER_H

#include "common/omCommon.h"
#include "common/om.hpp"
#include "zi/omMutex.h"
#include "zi/omUtility.h"
#include "datalayer/hdf5/omHdf5.h"

#include <QHash>
#include <QFileInfo>
#include <boost/make_shared.hpp>

class OmHdf5Manager : private om::singletonBase<OmHdf5Manager> {

 public:
	static void Delete(){
		zi::guard g(instance().mutex_);
		instance().hdf5Files_.clear();
	}

	static OmHdf5* getOmHdf5File(const std::string& fileNameAndPath,
								 const bool readOnly,
								 const om::Affinity aff)
	{
		QFileInfo fInfo(QString::fromStdString(fileNameAndPath));
		const std::string abs_fnpn = fInfo.absoluteFilePath().toStdString();

		return instance().doGetOmHdf5File(abs_fnpn, readOnly, aff);
	}


 private:
	OmHdf5Manager(){}
	~OmHdf5Manager(){}

	zi::mutex mutex_;
	std::map<std::string, boost::shared_ptr<OmHdf5> > hdf5Files_;

	OmHdf5* doGetOmHdf5File(const std::string& fnp, const bool readOnly, const om::Affinity aff)
	{
		zi::guard g(mutex_);

		if(hdf5Files_.count(fnp)){
			OmHdf5* f = hdf5Files_[fnp].get();
			f->aff_ = aff;
			return f;
		}

		boost::shared_ptr<OmHdf5> hdf5File(new OmHdf5(fnp, readOnly, aff));

		hdf5Files_[fnp] = hdf5File;
		return hdf5File.get();
	}

 	friend class zi::singleton<OmHdf5Manager>;
};

#endif
