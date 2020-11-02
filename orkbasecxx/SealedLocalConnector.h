//
// Created by arik halperin on 02/11/2020.
//

#ifndef OREKA_SEALEDLOCALCONNECTOR_H
#define OREKA_SEALEDLOCALCONNECTOR_H
#include <StdString.h>
#include <curl/curl.h>
#include <OrkBase.h>

class DLL_IMPORT_EXPORT_ORKBASE SealedLocalConnector{
    static SealedLocalConnector * s_instance;
    CURL *curl;

  public:
    SealedLocalConnector();
    ~SealedLocalConnector();
    void CallServer(CStdString & path, CStdString & data);
    void SendStartCall(CStdString & jsonMessage);
    void SendEndCall(CStdString & jsonMessage);
    void SendTranscription(CStdString & jsonMessage);
    static SealedLocalConnector * instance();

};


#endif //OREKA_SEALEDLOCALCONNECTOR_H
