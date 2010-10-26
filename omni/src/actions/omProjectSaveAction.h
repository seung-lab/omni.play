#ifndef OM_PROJECT_SAVE_ACTION_H
#define OM_PROJECT_SAVE_ACTION_H

#include "system/omAction.h"

class OmProjectSaveActionImpl;

class OmProjectSaveAction : public OmAction {

public:
	OmProjectSaveAction( );

private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string&);

	boost::shared_ptr<OmProjectSaveActionImpl> impl_;
};

#endif
