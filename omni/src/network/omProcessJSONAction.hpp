#ifndef OM_PROCESS_JSON_ACTION_HPP
#define OM_PROCESS_JSON_ACTION_HPP

template <typename T>
std::vector<T> getParamVec(const OmJson& json, const std::string name)
{
    boost::optional<std::vector<T> > params = json.Params<T>(name);
    if(!params){
        return std::vector<T>();;
    }

    return *params;
}

class OmProcessJSONAction {
public:
    virtual std::string Process(const OmJson& json) = 0;
};

class OmJSONListProjects : public OmProcessJSONAction {
public:
    std::string Process(const OmJson&){
        return "{ \"name\": \"hi from list projcets\", \"id\": 123 }";
    }

    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONListProjects();
    }
};

class OmJSONGetSlice : public OmProcessJSONAction {
public:
    std::string Process(const OmJson& json)
    {
        try {
            std::vector<int> params = getParamVec<int>(json, "slice_num");
            if(params.size() != 1){
                return "fail vector size";
            }

            const int sliceNum = params[0];
            std::cout << "sliceNum: " << sliceNum << "\n";

            return "good";

        } catch(...){
            return "fail exception";
        }
    }

    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONGetSlice();
    }
};

#endif
