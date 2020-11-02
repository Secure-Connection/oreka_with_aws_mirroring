//
// Created by arik halperin on 02/11/2020.
//

#ifndef OREKA_SEALEDLOCALCONNECTOR_H
#define OREKA_SEALEDLOCALCONNECTOR_H

#include "OrkClient.h"

class DLL_IMPORT_EXPORT_ORKBASE SealedLocalConnector : public OrkHttpClient{
    static SealedLocalConnector * s_instance;
    static SealedLocalConnector * instance();
    virtual bool Execute(SyncMessage&, AsyncMessage&, const CStdString&, int, const CStdString&, int, bool) {}
};


#endif //OREKA_SEALEDLOCALCONNECTOR_H
