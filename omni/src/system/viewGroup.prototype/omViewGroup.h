/// @cond DOXYGEN_IGNORE

#ifndef _OM_VIEWGROUP_H_
#define _OM_VIEWGROUP_H_

#include "omLayer.h"
#include "omGenericManager.h"

/**
 * The view list item called the OmViewGroup holds the layers and all
 *  state for that specific view.
 * 
 *  Matt Wimer mwimer@mit.edu  3/18/2010
 */

class OmViewGroup : public OmManageableObject {

public:

	OmLayer& AddLayer ();
	void RemoveBinder (OmId binder);
	void RemoveBinder (OmBinder& binder);
	set<OmBinder>& GetBinder();

	void MoveLayerUp (OmId layer);
	void MoveLayerUp (OmLayer& layer);
	void MoveLayerDown (OmId layer);
	void MoveLayerDown (OmLayer& layer);

	int GetLayerCount ();

        //viewbox state
        void SetViewSliceMin(ViewType, Vector2<float>, bool postEvent = true);
        Vector2<float> GetViewSliceMin(ViewType);

        void SetViewSliceMax(ViewType, Vector2<float>, bool postEvent = true);
        Vector2<float> GetViewSliceMax(ViewType);

        SpaceCoord GetViewDepthCoord();
        void SetViewSliceDepth(ViewType, float, bool postEvent = true);
        float GetViewSliceDepth(ViewType);

        void SetZoomLevel(Vector2<int>);
        Vector2<int> GetZoomLevel();

        void SetPanDistance(ViewType, Vector2f, bool postEvent = true);
        Vector2f GetPanDistance(ViewType);

        // slices
        void SetSliceState(OmSlicePlane plane, bool enabled);

        void SetViewSliceDataFormat(int bytesPerSample, int samplesPerPixel);
        void SetViewSlice(const OmSlicePlane plane, const Vector3<int> &dim, unsigned char *data);

private:
	OmGenericManager <OmLayer> mLayerManager;

};

//        ar & mLayerManager;



#endif

/// @endcond
