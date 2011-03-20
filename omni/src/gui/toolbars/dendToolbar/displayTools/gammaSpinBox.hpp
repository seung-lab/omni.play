#ifndef GAMMA_SPIN_BOX_HPP
#define GAMMA_SPIN_BOX_HPP

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/displayTools/displayTools.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "tiles/cache/omTileCache.h"
#include "utility/image/omFilterImage.hpp"
#include "viewGroup/omViewGroupState.h"

#include <limits>

class GammaSpinBox : public OmDoubleSpinBox {
Q_OBJECT

public:
	GammaSpinBox(DisplayTools* d)
		: OmDoubleSpinBox(d, om::UPDATE_AS_TYPE)
		, mParent(d)
	{
		setSingleStep(0.05);
		setRange(-5, 5);
		setInitialGUIThresholdValue();
	}

private:
	DisplayTools *const mParent;

	void actUponThresholdChange(float threshold)
	{
		OmImageFilter::SetGamma(threshold);
		OmTileCache::ClearChannel();
		std::cout << "gamma changed to " << threshold << "\n";
	}

	void setInitialGUIThresholdValue()
	{
        setValue(OmImageFilter::GetGamma());
	}

	OmViewGroupState* vgs() const {
		return mParent->getViewGroupState();
	}
};

#endif
