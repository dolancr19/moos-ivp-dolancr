/************************************************************/
/*    NAME: Christopher Dolan                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef CommunicationAngle_HEADER
#define CommunicationAngle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>

using namespace std;

class CommunicationAngle : public CMOOSApp
{
 public:
   CommunicationAngle();
   ~CommunicationAngle();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();

 private: // Configuration variables

 private: // State variables
   stringstream m_cnx;
   stringstream m_cny;
   stringstream m_cnd;
   stringstream m_cnh;
   stringstream m_cns;
   bool m_collaborator_switch = true;
   string m_vehicle_name;
   string m_collaborator_name;
   double m_nav_x;
   double m_nav_y;
   double m_nav_depth;
   double m_nav_heading;
   double m_nav_speed;
   double m_cnx_val;
   double m_cny_val;
   double m_cnd_val;
   double m_cns_val;
   double m_cnh_val;
   double m_range;
   double c_surface;
   double c_gradient;
   double m_radius;
   
};

#endif 
