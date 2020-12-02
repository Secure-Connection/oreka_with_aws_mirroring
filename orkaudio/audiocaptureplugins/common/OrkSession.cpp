#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
//#include "winsock2.h"
#endif
#include "OrkSession.h"
#include "LogManager.h"
#include "ConfigManager.h"


static LoggerPtr getLog() {
	static LoggerPtr s_log = Logger::getLogger("orksession");
	return s_log;
}

OrkSession::OrkSession(AcpConfig * config): m_config(config), m_telephoneEventPayloadType(config->m_rtpEventPayloadTypeDefaultValue)
{
	m_started = false;
	m_stopped = false;
	m_capturePort = "";
	m_trackingId = "";
	m_beginDate = 0;
	m_keepRtp = true;
	if(CONFIG.m_lookBackRecording == false)
	{
		m_keepRtp = false;
	}
	m_onDemand = false;
	m_startWhenReceiveS2 = false;
	m_nonLookBackSessionStarted = false;

	// DTMF related
	m_dtmfDigitString = "";
	m_currentRtpEventTs = 0;
	
	for (int i = 0; i < 32; i++){
		m_orekaRtpPayloadTypeMap[i] = i+96;
	}
	if(config->m_sessionStartsOnS2ActivityDb != 0){
		FilterRef decoder;
		for(int pt = 0; pt < RTP_PAYLOAD_TYPE_MAX; pt++)
		{
			decoder = FilterRegistry::instance()->GetNewFilter(pt);
			m_decoders.push_back(decoder);
		}
	}
}

bool OrkSession::ShouldSwapChannels()
{
	bool s1_shouldBeLocal = m_config->m_rtpS1S2MappingDeterministicS1IsLocal == true;
	bool s2_shouldBeLocal = !s1_shouldBeLocal;

	//
	// If Media Gateways have been defined, check the ip source against the
	// media gateways to determine whether the flow is "local" or " remote"
	// Otherwise, check against localIp. Using media gateways is the preferred approach
	// as localIp follows the SIP endpoints, which are not necessarily the same as
	// the media endpoints
	if ( !m_config->m_mediaGateways.Empty())
	{
		bool s1_isMediaGateway = m_config->IsMediaGateway(m_lastRtpPacketSide1->m_sourceIp);
		bool s2_isMediaGateway = m_config->IsMediaGateway(m_lastRtpPacketSide2->m_sourceIp);

		if (s1_isMediaGateway && s1_shouldBeLocal) {
			m_logMsg = "s1 is a MediaGateway";
			return true;
		}

		if (s2_isMediaGateway && s2_shouldBeLocal) {
			m_logMsg = "s2 is a MediaGateway";
			return true;
		}
	}
	else
	{
		bool s1_isLocal = (unsigned int)m_lastRtpPacketSide1->m_sourceIp.s_addr == (unsigned int)m_localIp.s_addr;
		bool s2_isLocal = (unsigned int)m_lastRtpPacketSide2->m_sourceIp.s_addr == (unsigned int)m_localIp.s_addr;

		if (s2_isLocal && s1_shouldBeLocal) {
			m_logMsg = "s2 matches localip";
			return true;
		}

		if (s1_isLocal && s2_shouldBeLocal) {
			m_logMsg = "s1 matches localip";
			return true;
		}
	}


	return false;
}

void OrkSession::Start() {
}

void OrkSession::ReportMetadata() {
}

// ==========================================================================
//
// DetectChannel (RtpPacketInfoRef& rtpPacket)
//
// Detects and returns the correct channel number for the rtp packet
// it starts the session if session is set to start with the first s2.
//
// =========================================================================

int OrkSession::DetectChannel(RtpPacketInfoRef& rtpPacket, bool* pIsFirstPacket)
{
	CStdString logMsg;

    //LOG4CXX_INFO(getLog(), "DetectChannel Entry");

	if (pIsFirstPacket)
	{
		*pIsFirstPacket = false;
	}

	if(m_lastRtpPacketSide1.get() == NULL)
	{
      //  LOG4CXX_INFO(getLog(), "DetectChannel 0x1");
		// First RTP packet for side 1
		m_lastRtpPacketSide1 = rtpPacket;

		if(getLog()->isInfoEnabled())
		{
        //    LOG4CXX_INFO(getLog(), "DetectChannel 0x2");
			rtpPacket->ToString(logMsg);
			logMsg =  "[" + m_trackingId + "] 1st packet s1: " + logMsg;
			LOG4CXX_INFO(getLog(), logMsg);
		}
		if (pIsFirstPacket)
		{
          //  LOG4CXX_INFO(getLog(), "DetectChannel 0x3");
			*pIsFirstPacket = true;
		}
		return 1;
	}
	else if( rtpPacket->m_ssrc == m_lastRtpPacketSide1->m_ssrc && m_lastRtpPacketSide1->m_destIp.s_addr == rtpPacket->m_destIp.s_addr ) {
        //LOG4CXX_INFO(getLog(), "DetectChannel 0x4");
		return 1;
	}

	if(m_lastRtpPacketSide2.get() == NULL)
	{
        //LOG4CXX_INFO(getLog(), "DetectChannel 0x5");
		// First RTP packet for side 2
		m_lastRtpPacketSide2 = rtpPacket;

		if(getLog()->isInfoEnabled())
		{
          //  LOG4CXX_INFO(getLog(), "DetectChannel 0x6");
			rtpPacket->ToString(logMsg);
			logMsg =  "[" + m_trackingId + "] 1st packet s2: " + logMsg;
			LOG4CXX_INFO(getLog(), logMsg);
		}
		//We just ignore the first s2 packet if m_sessionStartsOnS2ActivityDb enable, since channel swapping could happen after first s1 and s2
		if(m_config->m_sessionStartsOnS2ActivityDb != 0)
		{
            //LOG4CXX_INFO(getLog(), "DetectChannel 0x7");
			//do nothing to not start the call
		}
		else if(CONFIG.m_discardUnidirectionalCalls && m_startWhenReceiveS2)
		{
        //    LOG4CXX_INFO(getLog(), "DetectChannel 0x8");
			Start();
			ReportMetadata();
			if (CONFIG.m_lookBackRecording == false)
			{
                LOG4CXX_INFO(getLog(), "DetectChannel 0x9");
				m_nonLookBackSessionStarted = true;
			}
		}
		if (pIsFirstPacket)
		{
          //  LOG4CXX_INFO(getLog(), "DetectChannel 0xA");
			*pIsFirstPacket = true;
		}
        //LOG4CXX_INFO(getLog(), "DetectChannel 0xB");

        return 2;
	}
	else if(rtpPacket->m_ssrc == m_lastRtpPacketSide2->m_ssrc && m_lastRtpPacketSide2->m_destIp.s_addr == rtpPacket->m_destIp.s_addr) {
        LOG4CXX_INFO(getLog(), "DetectChannel 0xC");

        if(!m_started && (m_config->m_sessionStartsOnS2ActivityDb != 0)){
            LOG4CXX_INFO(getLog(), "DetectChannel 0xD");

            AudioChunkDetails details;
			details.m_arrivalTimestamp = rtpPacket->m_arrivalTimestamp;
			details.m_numBytes = rtpPacket->m_payloadSize;
			details.m_timestamp = rtpPacket->m_timestamp;
			details.m_sequenceNumber = rtpPacket->m_seqNum;
			details.m_channel = 2;
			details.m_encoding = AlawAudio;
			details.m_numBytes = rtpPacket->m_payloadSize;
			if(rtpPacket->m_payloadType >= 96)
			{
                LOG4CXX_INFO(getLog(), "DetectChannel 0xE");

                details.m_rtpPayloadType = m_orekaRtpPayloadTypeMap[rtpPacket->m_payloadType-96];
			}
			else
			{
                LOG4CXX_INFO(getLog(), "DetectChannel 0xF");

                details.m_rtpPayloadType = rtpPacket->m_payloadType;
			}
			AudioChunkRef chunk(new AudioChunk());
			chunk->SetBuffer(rtpPacket->m_payload, details);
			double rmsVal = 0;
			if(chunk->GetEncoding() != PcmAudio)
			{
                LOG4CXX_INFO(getLog(), "DetectChannel 0x10");

                AudioChunkRef tmpChunkRef;
				FilterRef decoder;
				decoder = m_decoders.at(details.m_rtpPayloadType);
				if(decoder.get() != NULL)
				{
                    LOG4CXX_INFO(getLog(), "DetectChannel 0x11");

                    LOG4CXX_INFO(getLog(), "RTP Type:"+(int)details.m_rtpPayloadType);
					decoder->AudioChunkIn(chunk);
					decoder->AudioChunkOut(tmpChunkRef);
					if(tmpChunkRef.get())
					{
                        LOG4CXX_INFO(getLog(), "DetectChannel 0x12");

                        rmsVal = tmpChunkRef->ComputeRmsDb();
					}
				}	 else {
                    LOG4CXX_INFO(getLog(), "Could not find decoder");
				}
			}
			else
			{
                LOG4CXX_INFO(getLog(), "DetectChannel 0x13");

                rmsVal = chunk->ComputeRmsDb();
			}
			if(getLog()->isDebugEnabled())
			{
                LOG4CXX_INFO(getLog(), "DetectChannel 0x14");

                logMsg.Format("[%s] s2 db:%lf" ,m_trackingId, rmsVal);
				LOG4CXX_TRACE(getLog(), logMsg);
			}
			if(rmsVal > m_config->m_sessionStartsOnS2ActivityDb)
			{
                LOG4CXX_INFO(getLog(), "DetectChannel 0x15");

                Start();
				ReportMetadata();
				if (CONFIG.m_lookBackRecording == false)
				{
                    LOG4CXX_INFO(getLog(), "DetectChannel 0x16");

                    m_nonLookBackSessionStarted = true;
				}
			}
		}

        LOG4CXX_INFO(getLog(), "DetectChannel 0x17");

        return 2;
	}
    LOG4CXX_INFO(getLog(), "DetectChannel 0x18");


    return 0;
}
