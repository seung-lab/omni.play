#ifndef OM_DATE_TIME_HPP
#define OM_DATE_TIME_HPP

#include <QDateTime>

namespace om {
namespace datetime {

std::string cur(){
    return QDateTime::currentDateTime().toString("ddd MMMM d yy hh:mm:ss.zzz").toStdString();
}

} // namespace datetime
} // namespace om

#endif
