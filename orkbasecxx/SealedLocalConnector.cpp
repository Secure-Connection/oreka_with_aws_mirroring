//
// Created by arik halperin on 02/11/2020.
//

#include "SealedLocalConnector.h"

SealedLocalConnector * SealedLocalConnector::s_instance;

SealedLocalConnector * SealedLocalConnector::instance() {
    if(!SealedLocalConnector::s_instance) {
        SealedLocalConnector::s_instance = new SealedLocalConnector();
    }
    return s_instance;
}
