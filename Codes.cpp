// File: Codes.cpp
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#include "Codes.h"
#include <string>
using namespace std;

ACE_TCHAR* errorMessage(State code)
{
  string msg;
  switch(code)
    {
    case STATUS_CONFLICT:
      msg = ACE_TEXT(" (Data Conflict) ");break;
    case STATUS_FORBIDDEN:
      msg = ACE_TEXT(" (Forbidden) ");break;
    case STATUS_NETWORK_ERROR:
      msg = ACE_TEXT(" (Network Error) ");break;
    case STATUS_UNEXPECTED_DATA:
      msg = ACE_TEXT(" (Unexpected Data) ");break;
    case STATUS_INVALID_DATA:
      msg = ACE_TEXT(" (Invalid Data) ");break;
    default:
      msg = ACE_TEXT("unknown status");break;
    }
  return (char*)msg.c_str();
}
