#include "omView2d.h"
#include "segment/actions/segment/omSegmentSelectionAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/voxel/omVoxelSetValueAction.h"
#include "segment/actions/segment/omSegmentMergeAction.h"
#include "segment/omSegmentEditor.h"
#include "segment/omSegment.h"
#include "system/omStateManager.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omLocalPreferences.h"
#include "system/omSystemTypes.h"
#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "volume/omVolume.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include "project/omProject.h"

/**
 * \name Coordinate Converter Methods
 */
//\{
DataBbox OmView2d::SpaceToDataBbox(const SpaceBbox & spacebox)
{
	DataBbox new_data_box = OmVolume::NormToDataBbox(OmVolume::SpaceToNormBbox(spacebox));
	return new_data_box;
}

SpaceBbox OmView2d::DataToSpaceBbox(const DataBbox & databox)
{
	SpaceBbox new_space_box = OmVolume::NormToSpaceBbox(OmVolume::DataToNormBbox(databox));
	return new_space_box;
}

DataCoord OmView2d::SpaceToDataCoord(const SpaceCoord & spacec)
{
	DataCoord new_data_center = OmVolume::NormToDataCoord(OmVolume::SpaceToNormCoord(spacec));

	return new_data_center;
}

SpaceCoord OmView2d::DataToSpaceCoord(const DataCoord & datac)
{
	SpaceCoord new_space_center = OmVolume::NormToSpaceCoord(OmVolume::DataToNormCoord(datac));
	return new_space_center;
}

Vector2f OmView2d::ScreenToPanShift(Vector2i screenshift)
{
	Vector2f stretch= OmVolume::GetStretchValues(mViewType);
	Vector2i zoomLevel = OmStateManager::Instance()->GetZoomLevel();
	float factor = OMPOW(2,zoomLevel.x);
	float zoomScale = zoomLevel.y/10.0;
	float panx = screenshift.x;
	debug("cross","inside SreenToPanShift: panx: %f\n",panx);
	panx = panx/zoomScale/stretch.x;
	float pany = screenshift.y;
	pany = pany/zoomScale/stretch.y;
	return Vector2f(panx, pany);
}

SpaceCoord OmView2d::ScreenToSpaceCoord(ViewType viewType, const ScreenCoord & screenc)
{
	Vector2f mPanDistance = OmStateManager::Instance()->GetPanDistance(mViewType);
	debug("cross", "pan: %f, %f\n", mPanDistance.x, mPanDistance.y);
        Vector2f mZoomLevel = OmStateManager::Instance()->GetZoomLevel();         
	Vector3f mScale = OmVolume::Instance()->GetScale();                
	Vector2f stretch = OmVolume::GetStretchValues(mViewType);                  

	DataBbox extent = OmVolume::GetDataExtent();
	Vector3f dataScale;
	Vector3i scale = extent.getMax() - extent.getMin() + Vector3i::ONE;
	dataScale.x = (float) scale.x;
	dataScale.y = (float) scale.y;
	dataScale.z = (float) scale.z;
	debug("cross", "datascale: x,y,z: %f, %f, %f\n", dataScale.x, dataScale.y, dataScale.z);
	Vector3f datac;
	NormCoord normc = NormCoord(0,0,0);
        Vector3f result;
        float factor = OMPOW(2,mZoomLevel.x);                                                             
	debug("cross", "factor: %f\n", factor);
        float zoomScale = mZoomLevel.y;                                                                 
        switch(mViewType){                                                                              
        case XY_VIEW:                                                                                   
                normc.x = (((float)screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor)/dataScale.x;    
                normc.y = (((float)screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor)/dataScale.y;
		result = OmVolume::NormToSpaceCoord(normc);
		result.z = OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW);
                break;                                                                                  
        case XZ_VIEW:                                                                                   
                normc.x = (((float)screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor)/dataScale.x;  
                normc.z = (((float)screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor)/dataScale.z;
		result = OmVolume::NormToSpaceCoord(normc);
                result.y = OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW);
                break;                                                                                  
        case YZ_VIEW:
                normc.z = (((float)screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor)/dataScale.z;                           
                normc.y = (((float)screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor)/dataScale.y;
                result =  OmVolume::NormToSpaceCoord(normc);
                result.x = OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW);  
                break;                                                                                  
        default:                                                                                   
				 //assert(0);                                                                              
                break;                                                                                  
        }                         
	assert(!isnan(result.x));
	assert(!isnan(result.y));
	assert(!isnan(result.z));
        return result;        
}

ScreenCoord OmView2d::SpaceToScreenCoord(ViewType viewType, const SpaceCoord & spacec)
{
	NormCoord normCoord = OmVolume::SpaceToNormCoord(spacec);

	DataBbox extent = OmVolume::GetDataExtent();
	Vector3f scale;
	scale.x = (float)(extent.getMax().x - extent.getMin().x + 1);
	scale.y = (float)(extent.getMax().y - extent.getMin().y + 1);
	scale.z = (float)(extent.getMax().z - extent.getMin().z + 1);

	
	Vector3f datac = normCoord * scale;

	Vector2f mPanDistance = OmStateManager::Instance()->GetPanDistance(viewType);                  
        Vector2f mZoomLevel = OmStateManager::Instance()->GetZoomLevel(); 
	Vector2f stretch= OmVolume::GetStretchValues(mViewType);
	Vector2i result;
	float factor = OMPOW(2,mZoomLevel.x);
	float zoomScale = mZoomLevel.y;
	//debug ("cross", "factor: %f \n", factor);
	//debug ("cross", "zoomScale: %f \n", zoomScale);
	//debug ("cross", "mScale.(x,y,z):  (%f,%f,%f) \n",mScale.x,mScale.y,mScale.z);

	switch(viewType){
	case XY_VIEW:
		result.x = (int)((float)(datac.x/factor+mPanDistance.x)*zoomScale/10.0*stretch.x);
		result.y = (int)((float)(datac.y/factor+mPanDistance.y)*zoomScale/10.0*stretch.y);  
		break;
	case XZ_VIEW:
                result.x = (int)((float)(datac.x/factor+mPanDistance.x)*zoomScale/10.0*stretch.x);        
		result.y = (int)((float)(datac.z/factor+mPanDistance.y)*zoomScale/10.0*stretch.y);
		break;
	case YZ_VIEW:
		result.x = (int)((float)(datac.z/factor+mPanDistance.x)*zoomScale/10.0*stretch.x);       
	        result.y = (int)((float)(datac.y/factor+mPanDistance.y)*zoomScale/10.0*stretch.y);
		break;
	default:
		assert(0);
		break;
	}
	return result;
	
}

ScreenCoord OmView2d::DataToScreenCoord(ViewType viewType, const DataCoord & datac)
{
	Vector2f mPanDistance = OmStateManager::Instance()->GetPanDistance(viewType);                  
        Vector2f mZoomLevel = OmStateManager::Instance()->GetZoomLevel(); 
	Vector3f mScale = OmVolume::Instance()->GetScale();
	Vector2f stretch= OmVolume::GetStretchValues(mViewType);
	Vector2i result;
	float factor = OMPOW(2,mZoomLevel.x);
	float zoomScale = mZoomLevel.y;
	//debug ("cross", "factor: %f \n", factor);
	//debug ("cross", "zoomScale: %f \n", zoomScale);
	//debug ("cross", "mScale.(x,y,z):  (%f,%f,%f) \n",mScale.x,mScale.y,mScale.z);

	switch(viewType){
	case XY_VIEW:
		result.x = (int)((float)(datac.x/factor+mPanDistance.x)*zoomScale/10.0*stretch.x);
		result.y = (int)((float)(datac.y/factor+mPanDistance.y)*zoomScale/10.0*stretch.y);  
		break;
	case XZ_VIEW:
                result.x = (int)((float)(datac.x/factor+mPanDistance.x)*zoomScale/10.0*stretch.x);        
		result.y = (int)((float)(datac.z/factor+mPanDistance.y)*zoomScale/10.0*stretch.y);
		break;
	case YZ_VIEW:
		result.x = (int)((float)(datac.z/factor+mPanDistance.x)*zoomScale/10.0*stretch.x);       
	        result.y = (int)((float)(datac.y/factor+mPanDistance.y)*zoomScale/10.0*stretch.y);
		break;
	default:
		assert(0);
		break;
	}
	return result;
		
 }

DataCoord OmView2d::ScreenToDataCoord(ViewType viewType, const ScreenCoord & screenc)
{
	Vector2f mPanDistance =  OmStateManager::Instance()->GetPanDistance(mViewType);
        Vector2f mZoomLevel = OmStateManager::Instance()->GetZoomLevel();          Vector3f mScale = OmVolume::Instance()->GetScale();                
	Vector2f stretch= OmVolume::GetStretchValues(mViewType);                  
        Vector3i result;
        float factor = OMPOW(2,mZoomLevel.x);                                                             
        float zoomScale = mZoomLevel.y;                                                                 
        switch(mViewType){                                                                              
        case XY_VIEW:                                                                                   
                result.x = (int)((screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor);    
                result.y = (int)((screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor); 
		result.z = SpaceToDataCoord(Vector3f(0,0,OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW))).z;
                break;                                                                                  
        case XZ_VIEW:                                                                                   
                result.x = (int)((screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor);  
                result.z = (int)((screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor);
                result.y = SpaceToDataCoord(Vector3f(0,OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW),0)).y;
                break;                                                                                  
        case YZ_VIEW:
                result.z = (int)((screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor);                           
                result.y = (int)((screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor);                    
                result.x = SpaceToDataCoord(Vector3f(OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW),0,0)).x;  
                break;                                                                                  
        default:                                                                                   
				 //assert(0);                                                                              
                break;                                                                                  
        }                                                                                              
        return result; 
}

ScreenCoord OmView2d::NormToScreenCoord(ViewType viewType, const NormCoord & normc)
{
	//return DataToScreenCoord(NormToDataCoord(normc)); 
}

NormCoord OmView2d::ScreenToNormCoord(ViewType viewType, const ScreenCoord & screenc)
{
	//return DataToNormCoord(ScreenToDataCoord(screenc));  
}

//\}
