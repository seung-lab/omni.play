#ifndef OM_PROCESS_JSON_ACTION_HPP
#define OM_PROCESS_JSON_ACTION_HPP

class OmProcessJSONAction {
public:
    virtual std::string Process(const std::string& actionName) = 0;
};

class OmJSONListProjects : public OmProcessJSONAction {
public:
    std::string Process(const std::string&){
        return "hi from list projcets";
    }
    static OmProcessJSONAction* CreateInstance(){
        return new OmJSONListProjects();
    }
};

#endif
