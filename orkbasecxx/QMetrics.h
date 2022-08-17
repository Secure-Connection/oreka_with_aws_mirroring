//
// Created by arik halperin on 17/08/2022.
//

#ifndef OREKA_WITH_AWS_MIRRORING_QMETRICS_H
#define OREKA_WITH_AWS_MIRRORING_QMETRICS_H

#include "StdString.h"
#include <cstring>

class QMetricsFunctor {
public:
    virtual CStdString FinishCall(CStdString local, CStdString remote)=0;
};


class QMetricsProxy {
    static QMetricsFunctor * finishCallPtr;
public:
    static CStdString FinishCall(CStdString & local_party, CStdString & remote_party);
    static void SetFinishCallFunctor(QMetricsFunctor * finishCallFunctor);
};


#endif //OREKA_WITH_AWS_MIRRORING_QMETRICS_H
