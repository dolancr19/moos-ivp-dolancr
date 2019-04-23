/************************************************************/
/*    NAME: Christopher Dolan                               */
/*    CONTACT: dolancr@mit.edu                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.h                            */
/*    DATE: 07 March 2019                                   */
/************************************************************/

#ifndef CommunicationAngle_HEADER
#define CommunicationAngle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>

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
   bool m_collaborator_switch;
   double surface_sound_speed;
   double sound_speed_gradient;
   double water_depth;
   double time_interval;
   
 private: // State variables
   string m_vehicle_name;
   string m_collaborator_name;
   double m_nav_x;
   double m_nav_y;
   double m_nav_depth;
   double m_nav_heading;
   double m_nav_speed;
   string m_cnx;
   double m_cnx_val;
   string m_cny;
   double m_cny_val;
   string m_cnd;
   double m_cnd_val;
   string m_cns;
   double m_cns_val;
   string m_cnh;
   double m_cnh_val;
   double m_elev_angle;
   double m_elev_angle_d;
   double m_transmission_loss;
   double m_elev_test;
   double m_new_x;
   double m_new_y;
   double m_z_s_test;
};
#endif 
