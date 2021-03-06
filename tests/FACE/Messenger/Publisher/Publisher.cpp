#include "../Idl/FaceMessage_TS.hpp"

#ifdef ACE_AS_STATIC_LIBS
# include "dds/DCPS/RTPS/RtpsDiscovery.h"
# include "dds/DCPS/transport/rtps_udp/RtpsUdp.h"
#endif

#include "ace/OS_NS_unistd.h"
#include "ace/Log_Msg.h"

static bool no_global_new = false;

#if defined(OPENDDS_SAFETY_PROFILE) && (defined(__GLIBC__) || defined(ACE_HAS_EXECINFO_H))

#include <execinfo.h>

void* operator new(size_t sz)
#ifdef ACE_HAS_NEW_THROW_SPEC
  throw (std::bad_alloc)
#endif
 {
  if (no_global_new) {
    printf ("ERROR: call to global operator new\n");
  }
  return ::malloc(sz);
}

void operator delete(void* ptr) {
  ::free(ptr);
}

void* operator new[](size_t sz, const std::nothrow_t&)
#ifdef ACE_HAS_NEW_THROW_SPEC
  throw (std::bad_alloc)
#endif
{
  if (no_global_new) {
    printf ("ERROR: Publisher call to global operator new[]\n");
    void* addresses[32];
    int count = backtrace(addresses, 32);
    char** text = backtrace_symbols(addresses, count);
    for (int i = 0; i != count; ++i) {
      printf ("Publisher %s\n", text[i]);
    }
    ::free(text);
  }
  return ::malloc(sz);
}

void operator delete[](void* ptr) {
  ::free(ptr);
}
#endif

int ACE_TMAIN(int, ACE_TCHAR*[])
{
  FACE::RETURN_CODE_TYPE status;
  FACE::TS::Initialize("face_config.ini", status);
  if (status != FACE::RC_NO_ERROR) return static_cast<int>(status);

  no_global_new = true;

  FACE::CONNECTION_ID_TYPE connId;
  FACE::CONNECTION_DIRECTION_TYPE dir;
  FACE::MESSAGE_SIZE_TYPE size;
  FACE::TS::Create_Connection("pub", FACE::PUB_SUB, connId, dir, size,
                              FACE::INF_TIME_VALUE, status);
  if (status != FACE::RC_NO_ERROR) return static_cast<int>(status);

  FACE::CONNECTION_NAME_TYPE name = {};
  FACE::TRANSPORT_CONNECTION_STATUS_TYPE connectionStatus;
  FACE::TS::Get_Connection_Parameters(name, connId, connectionStatus, status);
  if (status != FACE::NOT_AVAILABLE) return static_cast<int>(status);

  //Check that name & connection validated ok
  FACE::CONNECTION_NAME_TYPE goodName = "pub";
  FACE::TS::Get_Connection_Parameters(goodName, connId, connectionStatus, status);
  if (status != FACE::NOT_AVAILABLE) {
    ACE_ERROR((LM_ERROR, "ERROR: Get_Connection_Parameters with goodName and good "
               "connection Id failed\n"));
  }

  //Check that wrong name & connection validated - INVALID_PARAM
  FACE::CONNECTION_NAME_TYPE badName = "wrong_pub";
  FACE::TS::Get_Connection_Parameters(badName, connId, connectionStatus, status);
  if (status != FACE::INVALID_PARAM) {
    ACE_ERROR((LM_ERROR, "ERROR: Get_Connection_Parameters with bad name and "
               "good connection Id failed to return INVALID_PARAM\n"));
  }

  //Check that name & wrong connection validated - INVALID_PARAM
  FACE::CONNECTION_ID_TYPE wrongConnectionId = 5;
  FACE::TS::Get_Connection_Parameters(goodName, wrongConnectionId, connectionStatus, status);
  if (status != FACE::INVALID_PARAM) {
    ACE_ERROR((LM_ERROR, "ERROR: Get_Connection_Parameters with good name and "
               "bad connection Id failed to return INVALID_PARAM\n"));
  }

  //Check that no name & no connection validated - INVALID_PARAM
  FACE::CONNECTION_NAME_TYPE noName = {};
  FACE::CONNECTION_ID_TYPE noConnectionId = 0;
  FACE::TS::Get_Connection_Parameters(noName, noConnectionId, connectionStatus, status);
  if (status != FACE::INVALID_PARAM) {
    ACE_ERROR((LM_ERROR, "ERROR: Get_Connection_Parameters with no name and no "
               "connection Id failed to return INVALID_PARAM\n"));
  }
  FACE::CONNECTION_ID_TYPE getConnectionId = 0;
  FACE::TS::Get_Connection_Parameters(name, getConnectionId, connectionStatus, status);
  if (status != FACE::NOT_AVAILABLE) return static_cast<int>(status);


  ACE_OS::sleep(5); // connection established with Subscriber

  Messenger::Message msg = {"Hello, world.", 0, 0};

#ifdef ACE_HAS_CDR_FIXED
  msg.deci = FACE::Fixed("987.654");
#endif

  FACE::TRANSACTION_ID_TYPE txn;
  ACE_DEBUG((LM_INFO, "Publisher: about to Send_Message()\n"));
  FACE::TS::Send_Message(connId, FACE::INF_TIME_VALUE, txn, msg, size, status);
  if (status != FACE::RC_NO_ERROR) return static_cast<int>(status);

  FACE::TS::Get_Connection_Parameters(name, connId, connectionStatus, status);
  if (status != FACE::RC_NO_ERROR) return static_cast<int>(status);

  if (connectionStatus.MESSAGE != 1
      || connectionStatus.LAST_MSG_VALIDITY != FACE::VALID
      || connectionStatus.WAITING_PROCESSES_OR_MESSAGES != 0
      || connectionStatus.CONNECTION_DIRECTION != FACE::SOURCE
      || connectionStatus.REFRESH_PERIOD != 0) {
    ACE_ERROR((LM_ERROR, "ERROR: unexpected value in connection "
               "parameters after sending\n"));
    return EXIT_FAILURE;
  }

  ACE_OS::sleep(15); // Subscriber receives message

  FACE::TS::Destroy_Connection(connId, status);
  if (status != FACE::RC_NO_ERROR) return static_cast<int>(status);

  return EXIT_SUCCESS;
}
