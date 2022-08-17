//
// Created by arik halperin on 17/08/2022.
//

#include "QMetrics.h"


QMetricsFinishCallPtr QMetricsProxy::finishCallPtr=NULL;

CStdString QMetricsProxy::FinishCall(CStdString & local_party, CStdString & remote_party) {
    if(finishCallPtr) {
        return finishCallPtr(local_party, remote_party);
    }
}

void QMetricsProxy::SetFinishCallPtr(QMetricsFinishCallPtr finish_call_function) {
    finishCallPtr = finish_call_function;
}
