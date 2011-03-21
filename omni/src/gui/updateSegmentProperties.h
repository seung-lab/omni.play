#ifndef UPDATE_SEGMENT_PROPERTIES_DIALOG_HPP
#define UPDATE_SEGMENT_PROPERTIES_DIALOG_HPP

#include "zi/omUtility.h"
#include "common/omBoost.h"

/**
 * Update open dialog (for instance, when user selects a different segment,
 *   change any open segment dialog to reflect the new segment...)
 */

class InspectorProperties;
class UpdateSegmentPropertiesDialogImpl;

class UpdateSegmentPropertiesDialog
    : private om::singletonBase<UpdateSegmentPropertiesDialog>
{
private:
    boost::scoped_ptr<UpdateSegmentPropertiesDialogImpl> impl_;

public:
    static void SetInspectorProperties(InspectorProperties* ip);
    static void Delete();

private:
    UpdateSegmentPropertiesDialog();
    ~UpdateSegmentPropertiesDialog();

    friend class zi::singleton<UpdateSegmentPropertiesDialog>;
};

#endif
