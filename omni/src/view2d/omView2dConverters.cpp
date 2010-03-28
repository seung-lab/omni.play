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
	Vector2f mPanDistance = GetPanDistance(mViewType);
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
	assert(!std::isnan(result.x));
	assert(!std::isnan(result.y));
	assert(!std::isnan(result.z));
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

	Vector2f mPanDistance = GetPanDistance(viewType);                  
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
	Vector2f mPanDistance = GetPanDistance(viewType);                  
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
	Vector2f mPanDistance =  GetPanDistance(mViewType);
        Vector2f mZoomLevel = OmStateManager::Instance()->GetZoomLevel();
	Vector3f mScale = OmVolume::Instance()->GetScale();                
	Vector2f stretch = OmVolume::GetStretchValues(mViewType);                  
	float factor = OMPOW(2,mZoomLevel.x);                                                             
        float zoomScale = mZoomLevel.y;                                                                 
        
	Vector3i result;
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
        }                                                                                              

        return result; 
}

DataCoord OmView2d::ToDataCoord(int xMipChunk, int yMipChunk, int mDataDepth)
{
	DataCoord this_data_coord;
	switch (mViewType) {
	case XY_VIEW:
		this_data_coord = DataCoord(xMipChunk, yMipChunk, mDataDepth);
		break;
	case XZ_VIEW:
		this_data_coord = DataCoord(xMipChunk, mDataDepth, yMipChunk);
		break;
	case YZ_VIEW:
		this_data_coord = DataCoord(mDataDepth, yMipChunk, xMipChunk);
		break;
	}
	return this_data_coord;
}

int OmView2d::GetDepthToDataSlice(ViewType viewType)
{
	SpaceCoord depthCoord = OmStateManager::GetViewDepthCoord();
	DataCoord dataCoord = SpaceToDataCoord(depthCoord);
	switch(viewType){
	case XY_VIEW:	return dataCoord.z;
	case XZ_VIEW:	return dataCoord.y;
	case YZ_VIEW:	return dataCoord.x;
	default: assert(0);
	}
}

void OmView2d::SetDataSliceToDepth(ViewType viewType, int slice)
{
	DataCoord dataCoord = DataCoord(0, 0, 0);
	SpaceCoord depthCoord = SpaceCoord(0.0, 0.0, 0.0);
	switch(viewType){
	case XY_VIEW:
		dataCoord.z = slice;
		depthCoord = DataToSpaceCoord(dataCoord);
		OmStateManager::SetViewSliceDepth( viewType, depthCoord.z);
		break;
	case XZ_VIEW:
		dataCoord.y = slice;
		depthCoord = DataToSpaceCoord(dataCoord);
		OmStateManager::SetViewSliceDepth( viewType, depthCoord.y);		
		break;
	case YZ_VIEW:
		dataCoord.x = slice;
		depthCoord = DataToSpaceCoord(dataCoord);
		OmStateManager::SetViewSliceDepth( viewType, depthCoord.x);		
		break;
	default: assert(0);
	}
}

int OmView2d::GetDepthToDataMax(ViewType viewType)
{
	NormCoord normCoord = NormCoord(1.0,1.0,1.0);
	
	DataCoord maxCoord = OmVolume::NormToDataCoord(normCoord);
	switch(viewType){
	case XY_VIEW:	return maxCoord.z;
	case XZ_VIEW:	return maxCoord.y;
	case YZ_VIEW:	return maxCoord.x;
	default: assert(0);
	}
}

Vector2f OmView2d::GetPanDistance(ViewType viewType)
{
	Vector2f pd;
	Vector2f better;
        Vector2f mZoomLevel = OmStateManager::Instance()->GetZoomLevel();
        Vector3f mScale = OmVolume::Instance()->GetScale();
        Vector2f stretch= OmVolume::GetStretchValues(mViewType);
        float factor = OMPOW(2,mZoomLevel.x);
        float zoomScale = mZoomLevel.y;


 	pd = OmStateManager::Instance()->GetPanDistance(mViewType);
	debug("pan", "good: x,y:%f,%f\n", pd.x, pd.y);

        if (!OmLocalPreferences::getStickyCrosshairMode()){
		return pd;
	}

	// Else we are in sticky crosshair mode.
	float x = OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW); 
	float y = OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW);
	float z = OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW);

	SpaceCoord mydepth = SpaceCoord(x, y, z);
	DataCoord mydataCoord = SpaceToDataCoord(mydepth);
	debug("pan", "1: dc:x,y,z:%i,%i,%i\n", mydataCoord.x, mydataCoord.y, mydataCoord.z);

	float panx = (mTotalViewport.width/2.0)/(zoomScale*stretch.x/10.0);
	float pany = (mTotalViewport.height/2.0)/(zoomScale*stretch.y/10.0);

	debug("pan", "pan:x,y:%f,%f\n", panx, pany);
	
        switch(viewType){
        case XY_VIEW:
		better.x = (panx-mydataCoord.x/factor);
		better.y = (pany-mydataCoord.y/factor);
		break;
	case XZ_VIEW:
		better.x = (panx-mydataCoord.x/factor);
		better.y = (pany-mydataCoord.z/factor);
		break;
	case YZ_VIEW:
		better.x = (panx-mydataCoord.z/factor);
		better.y = (pany-mydataCoord.y/factor);
		break;
	default:
		break;
	}

	debug("pan", "better: x,y:%f,%f\n", better.x, better.y);

	return better;
	return pd;
}
//\}

