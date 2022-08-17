//
// Created by arik halperin on 17/08/2022.
//

#ifndef OREKA_WITH_AWS_MIRRORING_QMETRICS_H
#define OREKA_WITH_AWS_MIRRORING_QMETRICS_H

#include "StdString.h"
#include <cstring>

typedef extern CStdString (*QMetricsFinishCallPtr)(CStdString &, CStdString);

class QMetricsProxy {
    static QMetricsFinishCallPtr finishCallPtr;
public:
    static CStdString FinishCall(CStdString & local_party, CStdString & remote_party);
    static void SetFinishCallPtr(QMetricsFinishCallPtr finish_call_function);
};


#endif //OREKA_WITH_AWS_MIRRORING_QMETRICS_H
