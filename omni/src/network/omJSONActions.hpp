#ifndef OM_PROCESS_JSON_ACTION_HPP
#define OM_PROCESS_JSON_ACTION_HPP

#include "network/omServiceObjects.hpp"

class OmProcessJSONAction {
public:
    std::string Process(const OmJson& json, OmServiceObjects* serviceObjects)
    {
        try {
            return doProcess(json, serviceObjects);

        } catch(...){
            return "fail exception";
        }
    }

private:
    virtual std::string doProcess(const OmJson&, OmServiceObjects*) = 0;
};

class OmJSONGetSliceChannel : public OmProcessJSONAction {
private:
    std::string doProcess(const OmJson& json, OmServiceObjects* serviceObjects)
    {
        boost::optional<int> sliceNum = json.GetValue<int>("slice_num");

        if(!sliceNum){
            return "fail";
        }

        return serviceObjects->MakeImgFileChannel(*sliceNum);
    }

public:
    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONGetSliceChannel();
    }
};

class OmJSONGetSliceSegmentation : public OmProcessJSONAction {
private:
    std::string doProcess(const OmJson& json, OmServiceObjects* serviceObjects)
    {
        boost::optional<int> sliceNum = json.GetValue<int>("slice_num");

        if(!sliceNum){
            return "fail";
        }

        return serviceObjects->MakeImgFileSegmentation(*sliceNum);
    }

public:
    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONGetSliceSegmentation();
    }
};

class OmJSONGetTileSegmentation : public OmProcessJSONAction {
private:
    std::string doProcess(const OmJson& json, OmServiceObjects* serviceObjects)
    {
        boost::optional<int> sliceNum = json.GetValue<int>("slice_num");
        boost::optional<int> tileX = json.GetValue<int>("tileX");
        boost::optional<int> tileY = json.GetValue<int>("tileY");

        if(!sliceNum || !tileX || !tileY){
            return "fail";
        }

        return serviceObjects->MakeTileFileSegmentation(*sliceNum, *tileX, *tileY);
    }

public:
    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONGetTileSegmentation();
    }
};

class OmJSONSelectSegment : public OmProcessJSONAction {
private:
    std::string doProcess(const OmJson& json, OmServiceObjects* serviceObjects)
    {
        boost::optional<int> segmentationID = json.GetValue<int>("segmentationID");
        boost::optional<int> sliceNum = json.GetValue<int>("slice_num");
        boost::optional<int> x = json.GetValue<int>("x");
        boost::optional<int> y = json.GetValue<int>("y");
        boost::optional<int> h = json.GetValue<int>("h");
        boost::optional<int> w = json.GetValue<int>("w");

        if(!segmentationID || !sliceNum || !x || !y || !w || !h){
            return "fail--invalid params";
        }

        return serviceObjects->SelectSegment(*segmentationID, *sliceNum, *x, *y, *w, *h);
    }

public:
    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONSelectSegment();
    }
};

class OmJSONGetMeshData : public OmProcessJSONAction {
private:
    std::string doProcess(const OmJson& json, OmServiceObjects* serviceObjects)
    {
        boost::optional<int> segmentationID = json.GetValue<int>("segmentationID");
        boost::optional<int> segID = json.GetValue<int>("segID");
        boost::optional<int> mipLevel = json.GetValue<int>("mip");
        boost::optional<int> x = json.GetValue<int>("x");
        boost::optional<int> y = json.GetValue<int>("y");
        boost::optional<int> z = json.GetValue<int>("z");

        if(!segmentationID || !segID || !mipLevel || !x || !y || !z){
            return "fail--invalid params";
        }

        return serviceObjects->GetMeshData(*segmentationID, *segID, *mipLevel, *x, *y, *z);
    }

public:
    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONGetMeshData();
    }
};

class OmJSONProjectInfo : public OmProcessJSONAction {
private:
    std::string doProcess(const OmJson&, OmServiceObjects* serviceObjects)
    {
        return serviceObjects->GetProjectData();
    }

public:
    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONProjectInfo();
    }
};

class OmJSONSegmentationDim : public OmProcessJSONAction {
private:
    std::string doProcess(const OmJson&, OmServiceObjects* serviceObjects)
    {
        return serviceObjects->GetSegmentationDim();
    }

public:
    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONSegmentationDim();
    }
};

class OmJSONChangeThreshold : public OmProcessJSONAction {
private:
    std::string doProcess(const OmJson& json, OmServiceObjects* serviceObjects)
    {
        boost::optional<int> segmentationID = json.GetValue<int>("segmentationID");
        boost::optional<int> thresholdInt = json.GetValue<int>("threshold");

        if(!segmentationID || !thresholdInt){
            return "fail";
        }

        const float threshold = static_cast<float>(*thresholdInt) / 100.0f;
        return serviceObjects->ChangeThreshold(*segmentationID, threshold);
    }

public:
    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONChangeThreshold();
    }
};

#endif
