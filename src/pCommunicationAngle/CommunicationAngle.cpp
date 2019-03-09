/************************************************************/
/*    NAME: Christopher Dolan                               */
/*    CONTACT: dolancr@mit.edu                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.cpp                          */
/*    DATE: 07 March 2019                                   */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "CommunicationAngle.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CommunicationAngle::CommunicationAngle()
{
   m_nav_x=0;
   m_nav_y=0;
   m_nav_depth=0;
   m_nav_heading=0;
   m_nav_speed=0;
   m_cnx_val=0;
   m_cny_val=0;
   m_cnd_val=0;
   m_cns_val=0;
   m_cnh_val=0;
   m_elev_angle=0;
   m_elev_angle_d=0;
   m_transmission_loss=0;
   m_elev_test=0;
   m_new_x=0;
   m_new_y=0;
   m_z_s_test=0;
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

 string key   = msg.GetKey();
 double dval  = msg.GetDouble();
 string sval  = msg.GetString();

 if (key == "COLLABORATOR_NAME")
   m_collaborator_name = sval;
    if (m_collaborator_switch)
      {
        stringstream m_cnx_ss;
	m_cnx_ss << m_collaborator_name << "_NAV_X";
	m_cnx = m_cnx_ss.str();
        stringstream m_cny_ss;
	m_cny_ss << m_collaborator_name << "_NAV_Y";
	m_cny = m_cny_ss.str();
        stringstream m_cnd_ss;
	m_cnd_ss << m_collaborator_name << "_NAV_DEPTH";
	m_cnd = m_cnd_ss.str();
        stringstream m_cnh_ss;
	m_cnh_ss << m_collaborator_name << "_NAV_HEADING";
	m_cnh = m_cnh_ss.str();
        stringstream m_cns_ss;
	m_cns_ss << m_collaborator_name << "_NAV_SPEED";
	m_cns = m_cns_ss.str();
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
 if (key == m_cnx.c_str())
   m_cnx_val = dval;
 if (key == m_cny.c_str())
   m_cny_val = dval;
 if (key == m_cnd.c_str())
   m_cnd_val = dval;
 if (key == m_cnh.c_str())
   m_cnh_val = dval;
 if (key == m_cns.c_str())
   m_cns_val = dval;
 }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle::Iterate()
{
  //Calculate horizontal range  
  double range = sqrt(pow((m_nav_x-m_cnx_val),2)+pow((m_nav_y-m_cny_val),2));
  //Calculate angle of azimuth in the x-y plane
  double azi = atan(m_nav_y/m_nav_x);
  //Determine the center and radius of the circle describing the acoustic path
  double cx = (pow(range,2)-pow((m_cnd_val+(surface_sound_speed/sound_speed_gradient)),2)+pow((m_nav_depth+(surface_sound_speed/sound_speed_gradient)),2))/(2*range);
  double radius = sqrt(pow(cx,2)+pow((m_cnd_val+(surface_sound_speed/sound_speed_gradient)),2));
  //Calculate the transmission angle
  double transmission_angle = acos((surface_sound_speed+sound_speed_gradient*m_cnd_val)/(sound_speed_gradient*radius));
  //Calculate the maximum depth of the acoustic path
  double z_max = ((surface_sound_speed+sound_speed_gradient*m_cnd_val)/(sound_speed_gradient*cos(transmission_angle)))-(surface_sound_speed/sound_speed_gradient);
  //If the maximum acoustic path depth is less than maximum water depth, a direct path is available
  if (z_max < water_depth)
    {
      //Calculate the r(s) at z max
      double r_z_max = radius*sin(transmission_angle);
      //if ownship horizontal range is less the the horizontal range of z_max, the elevation angle is positive
      if (r_z_max > range)
        {
          m_elev_angle = acos((surface_sound_speed+(m_nav_depth*sound_speed_gradient))/(sound_speed_gradient*radius));
        }
      else
	{
	  m_elev_angle = -1*acos((surface_sound_speed+(m_nav_depth*sound_speed_gradient))/(sound_speed_gradient*radius));
	}
      //Convert radians to degrees for output
      m_elev_angle_d = m_elev_angle*180/M_PI;
      //Calculate the local pressure and transmission loss
      double arc_length = radius*(transmission_angle-m_elev_angle);
      double r_i = radius*sin(transmission_angle)-radius*sin(m_elev_angle);
      double r_ii = radius*sin(transmission_angle+.0001)-radius*sin(m_elev_angle+.0001);
      double J_s_local = (r_i/sin(m_elev_angle))*((r_ii-r_i)/.0001);
      double pressure_local = (1/(4*M_PI))*sqrt(abs(((surface_sound_speed+sound_speed_gradient*m_nav_depth)*cos(transmission_angle))/((surface_sound_speed+sound_speed_gradient*m_cnd_val)*J_s_local)));
      double pressure_ref =(1/(4*M_PI));
      m_transmission_loss = -20*log10(pressure_local/pressure_ref);
      //Assemble the required outputs and publish to MOOSDB
      stringstream path;
      path << "elev_angle=" << m_elev_angle_d << ",transmission_loss=" << m_transmission_loss <<",id=dolancr@mit.edu" << endl;
      stringstream location;
      location << "x=" << m_nav_x << ",y=" << m_nav_y << ",depth=" << m_nav_depth << ",id=dolancr@mit.edu" << endl;
      Notify("ACOUSTIC_PATH",path.str());
      Notify("CONNECTIVITY_LOCATION", location.str());
      return(true);
    }
  //Iterate through possible transmission angles to find circle closest to ownship location
  else
    {
      double range_compare = 0;
      for (unsigned int ii = 0;ii<=90;ii++)
        {
          double theta_test = ii*M_PI/180;
	  double radius_test = (surface_sound_speed)/(sound_speed_gradient*cos(theta_test));
	  double c_x_test = sqrt(pow(radius_test,2)-pow((m_cnd_val+(surface_sound_speed/sound_speed_gradient)),2));
	  double range_test = sqrt(pow((c_x_test-range),2)+pow(((-1*surface_sound_speed/sound_speed_gradient)-m_nav_depth),2));
          if (range_test-radius_test > range_compare)
	    {
	      range_compare = range_test-radius_test;
	    }
	    else
	      {
		//Calculate new elevation angle, r(s) and z(s) for closest point on cirlce
                double r_z_max_test = radius_test*sin(theta_test);
		if (r_z_max_test > range)
		  {
		    m_elev_test = atan(-1*(range-c_x_test)/(m_nav_depth-(-1*surface_sound_speed/sound_speed_gradient)));
		  }
		else
		  {
                    m_elev_test = -1*atan(-1*(range-c_x_test)/(m_nav_depth-(-1*surface_sound_speed/sound_speed_gradient)));
		  }
		double r_s_test = radius_test*sin(theta_test)-radius_test*sin(m_elev_test);
		m_z_s_test = radius_test*cos(theta_test)-(surface_sound_speed/sound_speed_gradient);
                //Calculate x and y positions of new aim point
		m_new_x = r_s_test*cos(azi);
		m_new_y = r_s_test*sin(azi);
                //Assemble the required outputs and publish to MOOSDB
		stringstream path;
                path << "elev_angle=NaN,transmission_loss=NaN,id=dolancr@mit.edu" << endl;
                stringstream location;
                location << "x=" << m_new_x << ",y=" << m_new_y << ",depth=" << m_z_s_test << ",id=dolancr@mit.edu" << endl;
                Notify("ACOUSTIC_PATH",path.str());
                Notify("CONNECTIVITY_LOCATION", location.str());
                return(true);
	      }
	  }
      }
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
  // Register for required variables;
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
      Register(m_cnx.c_str(),0);
      Register(m_cny.c_str(),0);
      Register(m_cnd.c_str(),0);
      Register(m_cnh.c_str(),0);
      Register(m_cns.c_str(),0);
    }
}
