#ifndef OM_MESH_PARAMS_HPP
#define OM_MESH_PARAMS_HPP

#include "zi/omUtility.h"

class OmMeshParams : private om::singletonBase<OmMeshParams> {
public:
	static void SetInitialError(const double error){
		instance().error_ = error;
	}
	static double GetInitialError() {
		return instance().error_;
	}

	static void SetDownScallingFactor(const int factor){
		instance().factor_ = factor;
	}
	static int GetDownScallingFactor() {
		return instance().factor_;
	}

private:
	OmMeshParams()
		: error_(1e-5)
		, factor_(2)
	{}

	double error_;
	int factor_;

	friend class zi::singleton<OmMeshParams>;
};

#endif
