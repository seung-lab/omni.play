
        mSystemMode = NAVIGATION_SYSTEM_MODE;
        mToolMode = ZOOM_MODE;

        //view slice data
        mViewSliceDataXY = NULL;
        mViewSliceDataYZ = NULL;
        mViewSliceDataXZ = NULL;

        mXYSliceEnabled = false;
        mYZSliceEnabled = false;
        mXZSliceEnabled = false;

        SpaceCoord depth = OmVolume::NormToSpaceCoord(NormCoord(0.5, 0.5, 0.5));
        mYZSlice[4] = depth.x;
        mXZSlice[4] = depth.y;
        mXYSlice[4] = depth.z;

        mXYPan[0] = 0.0;
        mXYPan[1] = 0.0;
        mYZPan[0] = 0.0;
        mYZPan[1] = 0.0;
        mXZPan[0] = 0.0;
        mXZPan[1] = 0.0;


/////////////////////////////////
///////          View Event

/*
 *      Set/Get minimum coordiante of view slice.
 */
void OmStateManager::SetViewSliceMin(ViewType plane, Vector2 < float >point, bool postEvent)
{
        switch (plane) {
        case XY_VIEW:
                Instance()->mXYSlice[0] = point.x;
                Instance()->mXYSlice[1] = point.y;
                break;
        case XZ_VIEW:
                Instance()->mXZSlice[0] = point.x;
                Instance()->mXZSlice[1] = point.y;
                break;
        case YZ_VIEW:
                Instance()->mYZSlice[0] = point.x;
                Instance()->mYZSlice[1] = point.y;
                break;
        default:
                assert(false);
        }

        if (postEvent)
                OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
}

Vector2 < float > OmStateManager::GetViewSliceMin(ViewType plane)
{
        switch (plane) {
        case XY_VIEW:
                return Vector2 < float >(&Instance()->mXYSlice[0]);
        case XZ_VIEW:
                return Vector2 < float >(&Instance()->mXZSlice[0]);
        case YZ_VIEW:
                return Vector2 < float >(&Instance()->mYZSlice[0]);
        default:
                assert(false);
        }
}


/*
 *      Set/Get maximum coordiante of view slice.
 */
void OmStateManager::SetViewSliceMax(ViewType plane, Vector2 < float >point, bool postEvent)
{
        switch (plane) {
        case XY_VIEW:
                Instance()->mXYSlice[2] = point.x;
                Instance()->mXYSlice[3] = point.y;
                break;
        case XZ_VIEW:
                Instance()->mXZSlice[2] = point.x;
                Instance()->mXZSlice[3] = point.y;
                break;
        case YZ_VIEW:
                Instance()->mYZSlice[2] = point.x;
                Instance()->mYZSlice[3] = point.y;
                break;
        default:
                assert(false);
        }

        if (postEvent)
                OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
}

Vector2 < float > OmStateManager::GetViewSliceMax(ViewType plane)
{
        switch (plane) {
        case XY_VIEW:
                return Vector2f(&Instance()->mXYSlice[2]);
        case XZ_VIEW:
                return Vector2f(&Instance()->mXZSlice[2]);
        case YZ_VIEW:
                return Vector2f(&Instance()->mYZSlice[2]);
        default:
                assert(false);
        }
}


/**
 *      Set/Get depth of view slice.
 */
void OmStateManager::SetViewSliceDepth(ViewType plane, float depth, bool postEvent)
{
        if (isnan(depth)) assert(0);
        switch (plane) {
        case XY_VIEW:
                Instance()->mXYSlice[4] = depth;
                break;
        case XZ_VIEW:
                Instance()->mXZSlice[4] = depth;
                break;
        case YZ_VIEW:
                Instance()->mYZSlice[4] = depth;
                break;
        default:
                assert(false);
        }

        if (postEvent)
                OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
}

SpaceCoord OmStateManager::GetViewDepthCoord()
{
        SpaceCoord spacec;
        spacec.x = Instance()->mYZSlice[4];
        spacec.y = Instance()->mXZSlice[4];
        spacec.z = Instance()->mXYSlice[4];
        return spacec;
}


float OmStateManager::GetViewSliceDepth(ViewType plane)
{
        switch (plane) {
        case XY_VIEW:
                return Instance()->mXYSlice[4];
        case XZ_VIEW:
                return Instance()->mXZSlice[4];
        case YZ_VIEW:
                return Instance()->mYZSlice[4];
        default:
                assert(false);
        }
}

/*
 *      Set/Get zoom level.
 */
void OmStateManager::SetZoomLevel(Vector2 < int >zoom)
{
        Instance()->zoom_level = zoom;

        OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_POS_CHANGE));
}

Vector2 < int > OmStateManager::GetZoomLevel()
{
        return Instance()->zoom_level;
}

/*
 *      Set/Get pan distance.
 */
void OmStateManager::SetPanDistance(ViewType plane, Vector2f pan, bool postEvent)
{
        switch (plane) {
        case XY_VIEW:
                Instance()->mXYPan[0] = pan.x;
                Instance()->mXYPan[1] = pan.y;
                break;
        case XZ_VIEW:
                Instance()->mXZPan[0] = pan.x;
                Instance()->mXZPan[1] = pan.y;
                break;
        case YZ_VIEW:
                Instance()->mYZPan[0] = pan.x;
                Instance()->mYZPan[1] = pan.y;
                break;
        default:
                assert(false);
        }

        if (postEvent)
                OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_POS_CHANGE));
}

Vector2f OmStateManager::GetPanDistance(ViewType plane)
{
        switch (plane) {
        case XY_VIEW:
                return Vector2f(Instance()->mXYPan[0], Instance()->mXYPan[1]);
        case XZ_VIEW:
                return Vector2f(Instance()->mXZPan[0], Instance()->mXZPan[1]);
        case YZ_VIEW:
                return Vector2f(Instance()->mYZPan[0], Instance()->mYZPan[1]);
        default:
                assert(false);
        }
}


/*
 *      Enable/disable orthogonal slice.
 */
void OmStateManager::SetSliceState(OmSlicePlane plane, bool enabled)
{
        switch (plane) {

        case SLICE_XY_PLANE:
                Instance()->mXYSliceEnabled = enabled;
                break;

        case SLICE_YZ_PLANE:
                Instance()->mYZSliceEnabled = enabled;
                break;

        case SLICE_XZ_PLANE:
                Instance()->mXZSliceEnabled = enabled;
                break;

        default:
                assert(false);
        }
}

/*
 *      Sets the data format for the slice image data.  This will automatically clear
 *      any image data previously set.
 */
void OmStateManager::SetViewSliceDataFormat(int bytesPerSample, int samplesPerPixel)
{
        //debug("genone","OmStateManager::SetViewSliceDataFormat");

        SetViewSlice(SLICE_XY_PLANE, Vector3 < int >::ZERO, NULL);
        SetViewSlice(SLICE_YZ_PLANE, Vector3 < int >::ZERO, NULL);
        SetViewSlice(SLICE_XZ_PLANE, Vector3 < int >::ZERO, NULL);

        Instance()->mViewSliceBytesPerSample = bytesPerSample;
        //Instance()->mViewSliceSamplesPerPixel = samplesPerPixel;      now single sample per pixel system
}

/*
 *      Sets slice dimensions and copies image data (deletes old image data if necessary).
 *      note: Uses previously specified format to perform deep copy of given data.
 */
void OmStateManager::SetViewSlice(const OmSlicePlane plane, const Vector3 < int >&dim, unsigned char *p_data)
{
        //debug("genone","OmStateManager::SetViewSlice");

        //get size of image data
        unsigned int data_size = Instance()->mViewSliceBytesPerSample * dim.x * dim.y * dim.z;
        //alloc and copy
        unsigned char *p_data_copy = new unsigned char[data_size];
        memcpy(p_data_copy, p_data, data_size);

        switch (plane) {

        case SLICE_XY_PLANE:
                Instance()->mViewSliceDimXY = dim;
                if (Instance()->mViewSliceDataXY)
                        delete Instance()->mViewSliceDataXY;
                Instance()->mViewSliceDataXY = p_data_copy;
                break;

        case SLICE_YZ_PLANE:
                Instance()->mViewSliceDimYZ = dim;
                if (Instance()->mViewSliceDataYZ)
                        delete Instance()->mViewSliceDataYZ;
                Instance()->mViewSliceDataYZ = p_data_copy;
                break;

        case SLICE_XZ_PLANE:
                Instance()->mViewSliceDimXZ = dim;
                if (Instance()->mViewSliceDataXZ)
                        delete Instance()->mViewSliceDataXZ;
                Instance()->mViewSliceDataXZ = p_data_copy;
                break;

        default:
                assert(false);
        }
}



/////////////////////////////////
///////          System Mode

OmSystemMode OmStateManager::GetSystemMode()
{
        return Instance()->mSystemMode;
}

void OmStateManager::SetSystemMode(const OmSystemMode new_mode)
{

        //return if no change
        if (new_mode == Instance()->mSystemMode)
                return;

        //set new mode
        Instance()->mSystemMode = new_mode;

        //post change event
        OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
}

/////////////////////////////////
///////          Tool Mode

OmToolMode OmStateManager::GetToolMode()
{
        return Instance()->mToolMode;
}

void OmStateManager::SetToolMode(const OmToolMode new_mode)
{
        //return if no change
        if (new_mode == Instance()->mToolMode)
                return;

        //set new mode
        Instance()->mToolMode = new_mode;

        //post tool mode change
        OmEventManager::PostEvent(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
}

/////////////////////////////////
///////       3d View Only: Transparency State

float OmStateManager::GetTransparencyAlpha()
{
        return Instance()->TRANSPARANCY_ALPHA;
}

void OmStateManager::SetTransparencyAlpha(float alpha)
{
        Instance()->TRANSPARANCY_ALPHA = alpha;
}


