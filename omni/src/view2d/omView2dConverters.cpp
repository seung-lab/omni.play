#include "view2d/omView2d.h"
#include "system/omLocalPreferences.h"
#include "system/viewGroup/omViewGroupState.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

/**
 * \name Coordinate Converter Methods
 */
//\{
DataBbox OmView2d::SpaceToDataBbox(const SpaceBbox & spacebox)
{
	return mVolume->SpaceToDataBbox(spacebox);
}

SpaceBbox OmView2d::DataToSpaceBbox(const DataBbox & databox)
{
	return mVolume->DataToSpaceBbox(databox);
}

Vector2f OmView2d::ScreenToPanShift(Vector2i screenshift)
{
	Vector2f stretch= mVolume->GetStretchValues(mViewType);
	Vector2i zoomLevel = mViewGroupState->GetZoomLevel();

	float zoomScale = zoomLevel.y/10.0;
	float panx = screenshift.x;
	debug("cross","inside SreenToPanShift: panx: %f\n",panx);
	panx = panx/zoomScale/stretch.x;
	float pany = screenshift.y;
	pany = pany/zoomScale/stretch.y;
	return Vector2f(panx, pany);
}

SpaceCoord OmView2d::ScreenToSpaceCoord(const ScreenCoord & screenc)
{
	Vector2f mPanDistance = GetPanDistance(mViewType);
	debug("cross", "pan: %f, %f\n", mPanDistance.x, mPanDistance.y);
        Vector2f mZoomLevel = mViewGroupState->GetZoomLevel();         
	Vector3f mScale = mVolume->GetScale();                
	Vector2f stretch = mVolume->GetStretchValues(mViewType);                  

	DataBbox extent = mVolume->GetDataExtent();
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
		result = mVolume->NormToSpaceCoord(normc);
		result.z = mViewGroupState->GetViewSliceDepth(XY_VIEW);
                break;                                                                                  
        case XZ_VIEW:                                                                                   
                normc.x = (((float)screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor)/dataScale.x;  
                normc.z = (((float)screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor)/dataScale.z;
		result = mVolume->NormToSpaceCoord(normc);
                result.y = mViewGroupState->GetViewSliceDepth(XZ_VIEW);
                break;                                                                                  
        case YZ_VIEW:
                normc.z = (((float)screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor)/dataScale.z;                           
                normc.y = (((float)screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor)/dataScale.y;
                result =  mVolume->NormToSpaceCoord(normc);
                result.x = mViewGroupState->GetViewSliceDepth(YZ_VIEW);  
                break;                                                                                  
        default:                                                                                   
				 //assert(0);                                                                              
                break;                                                                                  
        }                         
	assert(!ISNAN(result.x));
	assert(!ISNAN(result.y));
	assert(!ISNAN(result.z));
        return result;        
}

ScreenCoord OmView2d::SpaceToScreenCoord(ViewType viewType, const SpaceCoord & spacec)
{
	NormCoord normCoord = mVolume->SpaceToNormCoord(spacec);

	DataBbox extent = mVolume->GetDataExtent();
	Vector3f scale;
	scale.x = (float)(extent.getMax().x - extent.getMin().x + 1);
	scale.y = (float)(extent.getMax().y - extent.getMin().y + 1);
	scale.z = (float)(extent.getMax().z - extent.getMin().z + 1);

	
	Vector3f datac = normCoord * scale;

	Vector2f mPanDistance = GetPanDistance(viewType);                  
        Vector2f mZoomLevel = mViewGroupState->GetZoomLevel(); 
	Vector2f stretch= mVolume->GetStretchValues(mViewType);
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
        Vector2f mZoomLevel = mViewGroupState->GetZoomLevel(); 
	Vector3f mScale = mVolume->GetScale();
	Vector2f stretch= mVolume->GetStretchValues(mViewType);
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

DataCoord OmView2d::ScreenToDataCoord(const ScreenCoord & screenc)
{
	Vector2f mPanDistance =  GetPanDistance(mViewType);
        Vector2f mZoomLevel = mViewGroupState->GetZoomLevel();
	Vector3f mScale = mVolume->GetScale();                
	Vector2f stretch = mVolume->GetStretchValues(mViewType);                  
	float factor = OMPOW(2,mZoomLevel.x);                                                             
        float zoomScale = mZoomLevel.y;                                                                 
        
	Vector3i result;
	switch(mViewType){                                                                              
        case XY_VIEW:                                                                                   
                result.x = (int)((screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor);    
                result.y = (int)((screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor); 
		result.z = SpaceToDataCoord(Vector3f(0,0,mViewGroupState->GetViewSliceDepth(XY_VIEW))).z;
                break;                                                                                  
        case XZ_VIEW:                                                                                   
                result.x = (int)((screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor);  
                result.z = (int)((screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor);
                result.y = SpaceToDataCoord(Vector3f(0,mViewGroupState->GetViewSliceDepth(XZ_VIEW),0)).y;
                break;                                                                                  
        case YZ_VIEW:
                result.z = (int)((screenc.x/zoomScale/stretch.x*10.0-mPanDistance.x)*factor);
                result.y = (int)((screenc.y/zoomScale/stretch.y*10.0-mPanDistance.y)*factor);                    
                result.x = SpaceToDataCoord(Vector3f(mViewGroupState->GetViewSliceDepth(YZ_VIEW),0,0)).x;  
                break;                                                                                  
        }                                                                                              

        return result; 
}

int OmView2d::GetDepthToDataSlice(ViewType viewType)
{
	SpaceCoord depthCoord = mViewGroupState->GetViewDepthCoord();
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
		mViewGroupState->SetViewSliceDepth( viewType, depthCoord.z);
		break;
	case XZ_VIEW:
		dataCoord.y = slice;
		depthCoord = DataToSpaceCoord(dataCoord);
		mViewGroupState->SetViewSliceDepth( viewType, depthCoord.y);		
		break;
	case YZ_VIEW:
		dataCoord.x = slice;
		depthCoord = DataToSpaceCoord(dataCoord);
		mViewGroupState->SetViewSliceDepth( viewType, depthCoord.x);		
		break;
	default: assert(0);
	}
}

int OmView2d::GetDepthToDataMax(ViewType viewType)
{
	NormCoord normCoord = NormCoord(1.0,1.0,1.0);
	
	DataCoord maxCoord = mVolume->NormToDataCoord(normCoord);
	switch(viewType){
	case XY_VIEW:	return maxCoord.z;
	case XZ_VIEW:	return maxCoord.y;
	case YZ_VIEW:	return maxCoord.x;
	default: assert(0);
	}
}

//\}

