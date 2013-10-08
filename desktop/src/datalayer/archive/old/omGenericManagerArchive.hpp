#pragma once

#include "system/omGenericManager.hpp"
#include "zi/omUtility.h"
#include "datalayer/archive/old/utilsOld.hpp"
#include "datalayer/archive/old/omDataArchiveBoost.h"

#include <QDataStream>

class OmGenericManagerArchive {
public:
    template <class T>
    static void Load(QDataStream& in, OmGenericManager<T>& gm)
    {
        in >> gm.nextId_;
        in >> gm.size_;
        in >> gm.validSet_;
        in >> gm.enabledSet_;

        gm.vec_.resize(gm.size_, NULL);

        for(uint32_t i = 0; i < gm.validSet_.size(); ++i)
        {
            T* t = new T();
            in >> *t;
            gm.vec_[ t->GetID() ] = t;

            gm.vecValidPtrs_.push_back(t);
        }
    }
};

