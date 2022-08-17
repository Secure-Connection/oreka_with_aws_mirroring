//
// Created by arik halperin on 17/08/2022.
//

#include "QMetrics.h"


QMetricsFunctor * QMetricsProxy::finishCallPtr=NULL;

CStdString QMetricsProxy::FinishCall(CStdString & local_party, CStdString & remote_party) {
    if(finishCallPtr) {
        return finishCallPtr->FinishCall(local_party, remote_party);
    }
}

void QMetricsProxy::SetFinishCallFunctor(QMetricsFunctor *finishCallFunctor) {
    finishCallPtr = finishCallFunctor;
}
