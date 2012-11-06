#pragma once

#include "common/genericManager.hpp"
#include "zi/omUtility.h"

#include <QDataStream>

class OmGenericManagerArchive {
public:
    // template <class T>
    // static void Save(QDataStream& out, const om::common::GenericManager<T>& gm)
    // {
    //     out << gm.nextId_;
    //     out << gm.size_;
    //     out << gm.validSet_;
    //     out << gm.enabledSet_;

    //     FOR_EACH(iter, gm.validSet_){
    //         out << *gm.vec_[*iter];
    //     }
    // }

    template <class T>
    static void Load(QDataStream& in, om::common::GenericManager<T>& gm)
    {
    	int dummyInt;
        in >> dummyInt; //gm.nextId_;
        in >> dummyInt; //gm.size_;
        om::common::IDSet valid, enabled;
        in >> valid;//gm.validSet_;
        in >> enabled;//gm.enabledSet_;

        FOR_EACH(iter, valid)
        {
            T* t = new T(*iter);
            in >> *t;
            gm.Insert(*iter, t);
            gm.SetEnabled(*iter, enabled.count(*iter));
        }
    }
};

