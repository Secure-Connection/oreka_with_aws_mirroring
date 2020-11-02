//
// Created by arik halperin on 02/11/2020.
//

#include "SealedLocalConnector.h"

SealedLocalConnector * SealedLocalConnector::instance;

SaledLocalConnector * SealedLocalConnector::instance() {
    if(!SealedLocalConnector::instance) {
        SealedLocalConnector::instance = new SealedLocalConnector();
    }
    return instance;
}