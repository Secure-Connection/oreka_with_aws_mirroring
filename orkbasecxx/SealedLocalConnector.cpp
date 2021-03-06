//
// Created by arik halperin on 02/11/2020.
//

#include "SealedLocalConnector.h"

const CStdString BASE_URL = CStdString("http://localhost:6000/pbx_controller/v1");

SealedLocalConnector * SealedLocalConnector::s_instance;

SealedLocalConnector * SealedLocalConnector::instance() {
    if(!SealedLocalConnector::s_instance) {
        SealedLocalConnector::s_instance = new SealedLocalConnector();
    }
    return s_instance;
}


void SealedLocalConnector::CallServer(const CStdString path, const CStdString data) {
    CStdString actuallPath = BASE_URL+"/"+path;
    struct curl_slist *chunk = NULL;
    
    LOG4CXX_INFO(log, "Calling server");
    LOG4CXX_INFO(log, (CStdString("actuall_path:")+actuallPath).c_str());
    LOG4CXX_INFO(log, (CStdString("data:")+data).c_str());

    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_URL, actuallPath.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    CURLcode res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
}

SealedLocalConnector::SealedLocalConnector() {
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    log = OrkLogManager::Instance()->clientLog;
}

SealedLocalConnector::~SealedLocalConnector() {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void SealedLocalConnector::SendStartCall(const CStdString json ) {
    CallServer("start_call",json);
};

void SealedLocalConnector::SendStopCall(const CStdString json ) {
    CallServer("stop_call",json);
};

void SealedLocalConnector::SendTranscription(const CStdString json ) {
    CallServer("add_transcription_line",json);
};

