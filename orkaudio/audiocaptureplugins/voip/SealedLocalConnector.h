//
// Created by arik halperin on 02/11/2020.
//

#ifndef OREKA_SEALEDLOCALCONNECTOR_H
#define OREKA_SEALEDLOCALCONNECTOR_H

#include "OrkClient.h"

class SealedLocalConnector : public OrkHttpClient{
    static SealedLocalConnector * instance;
    static SealedLocalConnector * SealedLocalConnector::instance();
};


#endif //OREKA_SEALEDLOCALCONNECTOR_H
