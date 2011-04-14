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

class OmJSONSelectSegment : public OmProcessJSONAction {
private:
    std::string doProcess(const OmJson& json, OmServiceObjects* serviceObjects)
    {
        boost::optional<int> sliceNum = json.GetValue<int>("slice_num");
        boost::optional<int> x = json.GetValue<int>("x");
        boost::optional<int> y = json.GetValue<int>("y");
        boost::optional<int> h = json.GetValue<int>("h");
        boost::optional<int> w = json.GetValue<int>("w");

        if(!sliceNum || !x || !y || !w || !h){
            return "fail";
        }

        return serviceObjects->SelectSegment(1, *sliceNum, *x, *y, *w, *h);
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
        boost::optional<int> segID = json.GetValue<int>("segID");
        boost::optional<int> mipLevel = json.GetValue<int>("mip");
        boost::optional<int> x = json.GetValue<int>("x");
        boost::optional<int> y = json.GetValue<int>("y");
        boost::optional<int> z = json.GetValue<int>("z");

        if(!segID || !mipLevel || !x || !y || !z){
            return "fail";
        }

        return serviceObjects->GetMeshData(1, *segID, *mipLevel, *x, *y, *z);
    }

public:
    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONSelectSegment();
    }
};

#endif
