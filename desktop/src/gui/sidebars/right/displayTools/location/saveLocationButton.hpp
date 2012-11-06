
#include "gui/widgets/omButton.hpp"
#include "system/omAlphaVegasMode.hpp"
#include "datalayer/fs/omFile.hpp"
#include <QFile>
#include <viewGroup/omViewGroupView2dState.hpp>

namespace om {
namespace displayTools {
    
class saveLocationButton : public OmButton<QWidget> {
public:
    saveLocationButton(QWidget *dt, OmViewGroupState* vgs)
            : OmButton<QWidget>(dt,
                                "Save Location",
                                "save current location to file",
                                false)
            , vgs_(vgs)  
                                
    {
        file_.reset(new QFile(OmProject::FilesFolder() + "/locations"));
        file_->open(QIODevice::WriteOnly | QIODevice::Append);
        stream_.reset(new QTextStream(file_.get()));
    }

private:
    OmViewGroupState* vgs_;

    boost::scoped_ptr<QFile> file_;
    boost::scoped_ptr<QTextStream> stream_;
    

    void doAction()
    {
        float x = vgs_->View2dState()->GetScaledSliceDepth(om::common::ZY_VIEW);
        float y = vgs_->View2dState()->GetScaledSliceDepth(om::common::XZ_VIEW); 
        float z = vgs_->View2dState()->GetScaledSliceDepth(om::common::XY_VIEW); 
        (*stream_) << x << ", " << y << ", " << z << "\n";
        stream_->flush();
    }
};
    
} // namespace displayTools
} // namespace om
    