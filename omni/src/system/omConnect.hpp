#pragma once

#include <iostream>
#include <QObject>

namespace om {

inline static void connect(const QObject* sender,   const char* signal,
                           const QObject* receiver, const char* method)
{
    if(!QObject::connect(sender, signal, receiver, method, Qt::QueuedConnection)){
        std::cout << "failed making connection\n";
    }
}

} //namespace om

