#ifndef OM_NEWTORK_QT_HPP
#define OM_NEWTORK_QT_HPP

#include "common/omCommon.h"
#include <QTcpSocket>
#include <QtNetwork>

namespace om {
namespace networkQT {

static const int timeoutMS = 3000;

boost::optional<QString> ReadString(QTcpSocket* socket);
void WriteString(QTcpSocket* socket, const QString& str);

} // namespace networkQT
} // namespace om

#endif
