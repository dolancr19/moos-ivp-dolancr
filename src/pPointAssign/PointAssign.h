/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class PointAssign : public CMOOSApp
{
 public:
   PointAssign();
   ~PointAssign();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();

 private: // Configuration variables
   bool m_all_received;
   bool m_all_sent;
   bool m_first_name;
   bool m_first_ready;
   bool m_second_ready;
      
 private: // State variables
   bool m_assign_by_region;
   vector<string> m_out1;
   vector<string> m_out2;
   vector<string> m_vnames;
   vector<string> m_notify_name;
};

#endif 
