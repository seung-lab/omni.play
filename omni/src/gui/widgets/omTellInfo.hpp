#ifndef OM_TELL_INFO_HPP
#define OM_TELL_INFO_HPP

#include <QMessageBox>

class OmTellInfo : public QMessageBox {
public:
    OmTellInfo()
    {}

    OmTellInfo(const QString& text)
    {
        setText(text);
        exec();
    }
};

#endif
