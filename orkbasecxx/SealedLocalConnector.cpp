//
// Created by arik halperin on 02/11/2020.
//

#include "SealedLocalConnector.h"

char *BASE_URL = "http://localhost:6000/pbx_controller/v1"

SealedLocalConnector * SealedLocalConnector::s_instance;

SealedLocalConnector * SealedLocalConnector::instance() {
    if(!SealedLocalConnector::s_instance) {
        SealedLocalConnector::s_instance = new SealedLocalConnector();
    }
    return s_instance;
}


void SealedLocalConnector::CallServer(CStdString & path, CStdString & data) {
    CStdString actuallPath = BASE_URL+"/"+path;
    struct curl_slist *chunk = NULL;
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
}

SealedLocalConnector::~SealedLocalConnector() {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void SealedLocalConnector:SendStartCall(CStdString json &) {

};
