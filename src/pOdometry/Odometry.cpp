/************************************************************/
/*    NAME: Christopher Dolan                               */
/*    CONTACT: dolancr@mit.edu                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.cpp                                    */
/*    DATE: 14 February 2019                                */
/************************************************************/

#include <iterator>
#include <math.h>
#include "MBUtils.h"
#include "Odometry.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{
 bool   m_first_reading = true;
 double m_current_x = 0;
 double m_current_y = 0;
 double m_previous_x = 0;
 double m_previous_y = 0;
 double m_total_distance = 0;
}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++)
    {
      CMOOSMsg &msg = *p;

      // Obtain the MOOS variable name and value from the mail packet
      string key   = msg.GetKey();
      double dval  = msg.GetDouble();

      // Assign the value to the correct MOOS variable
      if (key == "NAV_X")
        m_current_x = dval;
      else if (key == "NAV_Y")
        m_current_y = dval;
      else
        cout << "Error!" << endl;
    }
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Odometry::OnConnectToServer()
{
   // register for variables here
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Odometry::Iterate()
{
  AppCastingMOOSApp::Iterate();

  double inc_distance;
    
  if (m_first_reading)
    {
      // Publish the inital value of total distance to MOOSDB
      Notify("ODOMETRY_DIST", m_total_distance);
      m_first_reading=false;
    }
  else
    {
      //Calculate the distance traveled since the last loop, add to the cumulative total and publish to MOOSDB
      m_total_distance = m_total_distance + sqrt(pow(m_current_x-m_previous_x, 2)+pow(m_current_y-m_previous_y, 2));
      Notify("ODOMETRY_DIST", m_total_distance);

      //Set previous variables to current variables in preparation for next loop
      m_previous_x = m_current_x;
      m_previous_y = m_current_y;
    }
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void Odometry::RegisterVariables()
{
  
  AppCastingMOOSApp::RegisterVariables();
  //Register for variables of interest
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
}

bool Odometry::buildReport()
{
  //Output total distance traveled for AppCasting
  m_msgs << "Total distance traveled: " << m_total_distance << endl;
  return(true);
}
