#ifndef OM_PROJECT_CLOSE_ACTION_H
#define OM_PROJECT_CLOSE_ACTION_H

#include "actions/details/omAction.h"

#include <boost/shared_ptr.hpp>

class OmProjectCloseActionImpl;

class OmProjectCloseAction : public OmAction {

public:
	OmProjectCloseAction( );

private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string&);

	boost::shared_ptr<OmProjectCloseActionImpl> impl_;
};

#endif
