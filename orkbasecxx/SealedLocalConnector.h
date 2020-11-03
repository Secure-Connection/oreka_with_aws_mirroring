//
// Created by arik halperin on 02/11/2020.
//

#ifndef OREKA_SEALEDLOCALCONNECTOR_H
#define OREKA_SEALEDLOCALCONNECTOR_H
#include <StdString.h>
#include <curl/curl.h>
#include <OrkBase.h>
#include <log4cxx/logger.h>
#include "LogManager.h"

class DLL_IMPORT_EXPORT_ORKBASE SealedLocalConnector{
    static SealedLocalConnector * s_instance;
    CURL *curl;
    log4cxx::LoggerPtr log;

  public:
    SealedLocalConnector();
    ~SealedLocalConnector();
    void CallServer(CStdString path, CStdString data);
    void SendStartCall(const CStdString jsonMessage);
    void SendStopCall(const CStdString jsonMessage);
    void SendTranscription(const CStdString jsonMessage);
    static SealedLocalConnector * instance();

};


#endif //OREKA_SEALEDLOCALCONNECTOR_H
