/************************************************************/
/*    NAME: Christopher Dolan                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "CommunicationAngle.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CommunicationAngle::CommunicationAngle()
{
}

//---------------------------------------------------------
// Destructor

CommunicationAngle::~CommunicationAngle()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CommunicationAngle::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

 string key   = msg.GetKey();
 double dval  = msg.GetDouble();
 string sval  = msg.GetString();

 if (key == "COLLABORATOR_NAME")
   m_collaborator_name = sval;
    if (m_collaborator_switch)
      {
        m_cnx << m_collaborator_name << "_NAV_X";
        m_cny << m_collaborator_name << "_NAV_Y";
        m_cnd << m_collaborator_name << "_NAV_DEPTH";
        m_cnh << m_collaborator_name << "_NAV_HEADING";
        m_cns << m_collaborator_name << "_NAV_SPEED";
        m_collaborator_switch = false;
	RegisterVariables();	
      }
 if (key == "VEHICLE_NAME")
   m_vehicle_name = sval;
 if (key == "NAV_X")
   m_nav_x = dval;
 if (key == "NAV_Y")
   m_nav_y = dval;
 if (key == "NAV_DEPTH")
   m_nav_depth = dval;
 if (key == "NAV_HEADING")
   m_nav_heading = dval;
 if (key == "NAV_SPEED")
   m_nav_speed = dval;
 if (key == m_cnx.str())
   m_cnx_val = dval;
 if (key == m_cny.str())
   m_cny_val = dval;
 if (key == m_cnd.str())
   m_cnd_val = dval;
 if (key == m_cnh.str())
   m_cnh_val = dval;
 if (key == m_cns.str())
   m_cns_val = dval;
 
 

 
   }

  
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle::Iterate()
{

  m_range = sqrt(pow((m_nav_x-m_cnx_val),2)+pow((m_nav_y-m_cny_val),2));




  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CommunicationAngle::OnStartUp()
{
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

void CommunicationAngle::RegisterVariables()
{
  // Register("FOOBAR", 0);
  if (m_collaborator_switch)
     Register("COLLABORATOR_NAME",0);  
  else
    {
      Register("VEHICLE_NAME",0);
      Register("NAV_X",0);
      Register("NAV_Y",0);
      Register("NAV_DEPTH",0);
      Register("NAV_HEADING",0);
      Register("NAV_SPEED",0);
      Register(m_cnx.str(),0);
      Register(m_cny.str(),0);
      Register(m_cnd.str(),0);
      Register(m_cnh.str(),0);
      Register(m_cns.str(),0);
    }
}

