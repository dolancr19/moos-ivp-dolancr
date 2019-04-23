/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardMgr.cpp                                        */
/*    DATE: Oct 26th 2012                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "HazardMgr.h"
#include "XYFormatUtilsHazard.h"
#include "XYFormatUtilsPoly.h"
#include "ACTable.h"
#include "NodeMessage.h"  // In the lib_ufield library
#include "NodeMessageUtils.h"  // In the lib_ufield library
#include <vector>

using namespace std;

//---------------------------------------------------------
// Constructor

HazardMgr::HazardMgr()
{
  // Config variables
  m_swath_width_desired = 25;
  m_pd_desired          = 0.7;

  // State Variables 
  m_sensor_config_requested = false;
  m_sensor_config_set   = false;
  m_swath_width_granted = 0;
  m_pd_granted          = 0;

  m_sensor_config_reqs = 0;
  m_sensor_config_acks = 0;
  m_sensor_report_reqs = 0;
  m_detection_reports  = 0;
  m_hazard_reports     = 0;

  m_summary_reports = 0;
  m_comms = false;
  m_current_time=0;
  m_previous=0;
  m_dup=0;
  m_new=0;
  m_x=0;
  m_y=0;
  m_nav_x=0;
  m_nav_y=0;

  m_missed_hazard=0;
  m_false_alarm=0;
  m_max_time=0;
  m_max_time_over=0;
  m_max_time_rate=0;
  m_mstr=true;
  m_comm_time=0;
  m_comm_request=0;
  m_distance=0;
  m_counter=0;
  m_detection=0;
  m_check=0;
  m_range=0;
  m_counter2=0;
  m_node_time=0;
  
  
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool HazardMgr::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString();
    double dval  = msg.GetDouble();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   
    if(key == "UHZ_CONFIG_ACK") 
      handleMailSensorConfigAck(sval);

    else if(key == "UHZ_OPTIONS_SUMMARY") 
      handleMailSensorOptionsSummary(sval);

    else if(key == "UHZ_DETECTION_REPORT")
      {
	m_detection=MOOSTime();
	m_counter+=1;
	handleMailDetectionReport(sval);
      }

    else if(key == "HAZARDSET_REQUEST") 
      handleMailReportRequest();

    else if(key == "UHZ_MISSION_PARAMS") 
      handleMailMissionParams(sval);

    else if(key == "VEHICLE_REPORT_REQUEST")
      handleMailVehicleReportRequest();

    else if (key == "VEHICLE_REPORT")
      handleMailVehicleReport(sval);

    else if(key == "NODE_REPORT")
      {
	m_comms=true;
	m_node_time=MOOSTime();
         
      }
    else if(key == "NAV_X")
      {
      
      
      m_nav_x=dval;}

    else if(key == "NAV_Y")
      m_nav_y=dval;
  
    //handles classification 
    else if(key == "UHZ_HAZARD_REPORT")
      {
	handleMailHazardReport(sval);
	m_vector_detection.push_back(sval.c_str());
      }
    else 
      reportRunWarning("Unhandled Mail: " + key);
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool HazardMgr::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool HazardMgr::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(!m_sensor_config_requested)
    postSensorConfigRequest();

  if(m_sensor_config_set)
    postSensorInfoRequest();

  EvaluatePD();

  m_current_time=MOOSTime();
  if(m_current_time-m_node_time>1)
    m_comms=false;

  
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool HazardMgr::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(true);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if((param == "swath_width") && isNumber(value)) {
      m_swath_width_desired = atof(value.c_str());
      handled = true;
    }
    else if(((param == "sensor_pd") || (param == "pd")) && isNumber(value)) {
      m_pd_desired = atof(value.c_str());
      handled = true;
    }
    else if(param == "report_name") {
      value = stripQuotes(value);
      m_report_name = value;
      handled = true;
    }
    else if(param == "region") {
      XYPolygon poly = string2Poly(value);
      if(poly.is_convex())
	m_search_region = poly;
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  m_hazard_set.setSource(m_host_community);
  m_hazard_set.setName(m_report_name);
  m_hazard_set.setRegion(m_search_region);
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void HazardMgr::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("UHZ_DETECTION_REPORT", 0);
  Register("UHZ_CONFIG_ACK", 0);
  Register("UHZ_OPTIONS_SUMMARY", 0);
  Register("UHZ_MISSION_PARAMS", 0);
  Register("HAZARDSET_REQUEST", 0);
  Register("VEHICLE_REPORT",0);
  Register("VEHICLE_REPORT_REQUEST",0);
  Register("NODE_REPORT",0);
  Register("UHZ_HAZARD_REPORT", 0);
  Register("NAV_X",0);
  Register("NAV_Y",0);
  m_previous=MOOSTime();
  m_comm_time=MOOSTime();
  
}

//---------------------------------------------------------
// Procedure: postSensorConfigRequest

void HazardMgr::postSensorConfigRequest()
{
  string request = "vname=" + m_host_community;
  
  //request += ",width=" + doubleToStringX(m_swath_width_desired,2);
  request += ",pd="    + doubleToStringX(m_pd_desired,2);

  m_sensor_config_requested = true;
  m_sensor_config_reqs++;
  Notify("UHZ_CONFIG_REQUEST", request);
}

//---------------------------------------------------------
// Procedure: postSensorInfoRequest

void HazardMgr::postSensorInfoRequest()
{
  string request = "vname=" + m_host_community;

  m_sensor_report_reqs++;
  Notify("UHZ_SENSOR_REQUEST", request);
}

//---------------------------------------------------------
// Procedure: handleMailSensorConfigAck

bool HazardMgr::handleMailSensorConfigAck(string str)
{
  // Expected ack parameters:
  string vname, width, pd, pfa, pclass;
  
  // Parse and handle ack message components
  bool   valid_msg = true;
  string original_msg = str;

  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "vname")
      vname = value;
    else if(param == "pd")
      pd = value;
    else if(param == "width")
      width = value;
    else if(param == "pfa")
      pfa = value;
    else if(param == "pclass")
      pclass = value;
    else
      valid_msg = false;       

  }


  if((vname=="")||(width=="")||(pd=="")||(pfa=="")||(pclass==""))
    valid_msg = false;
  
  if(!valid_msg)
    reportRunWarning("Unhandled Sensor Config Ack:" + original_msg);

  
  if(valid_msg) {
    m_sensor_config_set = true;
    m_sensor_config_acks++;
    m_swath_width_granted = atof(width.c_str());
    m_pd_granted = atof(pd.c_str());
  }

  return(valid_msg);
}

//---------------------------------------------------------
// Procedure: handleMailDetectionReport
//      Note: The detection report should look something like:
//            UHZ_DETECTION_REPORT = vname=betty,x=51,y=11.3,label=12 

bool HazardMgr::handleMailDetectionReport(string str)
{
  
	  
      
  m_detection_reports++;
  
  XYHazard new_hazard = string2Hazard(str);

  string hazlabel = new_hazard.getLabel();
  
  if(hazlabel == "") {
    reportRunWarning("Detection report received for hazard w/out label");
    return(false);
  }

  int ix = m_detection_set.findHazard(hazlabel);
  if(ix == -1)
    m_detection_set.addHazard(new_hazard);
  else
    m_detection_set.setHazard(ix, new_hazard);

  string event = "New Detection, label=" + new_hazard.getLabel();
  event += ", x=" + doubleToString(new_hazard.getX(),1);
  event += ", y=" + doubleToString(new_hazard.getY(),1);

  reportEvent(event);

  string req = "vname=" + m_host_community + ",label=" + hazlabel;

  Notify("UHZ_CLASSIFY_REQUEST", req);

  return(true);
}


//---------------------------------------------------------
// Procedure: handleMailReportRequest

void HazardMgr::handleMailReportRequest()
{
  m_summary_reports++;

  //m_hazard_set.findMinXPath(20);
  m_hazard_set.clear();
  m_hazard_set.setSource(m_host_community);
  m_hazard_set.setName(m_report_name);
  m_hazard_set.setRegion(m_search_region);
  //unsigned int count    = m_hazard_set.findMinXPath(20);

     
  unsigned int vsize = m_classified.size(); //size of classified detections
  //m_test=intToString(vsize);

  //cycle through each element of the classified array
  for (unsigned int i=0; i<vsize; i++)
    {
      XYHazard new_hazard = m_classified.getHazard(i);
      string haztype=  new_hazard.getType();
         
      vector<string> hazvector = parseString(haztype, '#');
      int haz_count=0;
      int ben_count=0;
      //count different classification of the same object
      for (unsigned int j=0; j<hazvector.size(); j++)
	{
	  if (hazvector[j] == "hazard")
	    haz_count += 1;
	  else if (hazvector[j] == "benign")
	    ben_count +=1;
	}
      //set final classification type
      
      if (haz_count >= ben_count)
	{
	  new_hazard.setType("hazard");
	  m_hazard_set.addHazard(new_hazard);}
        
	  else if (haz_count < ben_count)
	  new_hazard.setType("benign");

       // find the hazard label and insert it into hazard set  
      string hazlabel = new_hazard.getLabel();
       if (new_hazard.getType() == "hazard")
      	{
        int ix = m_hazard_set.findHazard(hazlabel);
        if(ix == -1)
          {
            m_hazard_set.addHazard(new_hazard);
              //string event = "New hazard added, label=" + new_hazard.getLabel();
              //event += ", x=" + doubleToString(new_hazard.getX(),1);
              //event += ", y=" + doubleToString(new_hazard.getY(),1);
              //event += ", type=" + new_hazard.getType();
	      //reportEvent(event);
          }
        else
          {
            m_hazard_set.setHazard(ix, new_hazard);
              //string event = "Old hazard updated, label=" + new_hazard.getLabel();
              //event += ", x=" + doubleToString(new_hazard.getX(),1);
              //event += ", y=" + doubleToString(new_hazard.getY(),1);
              //event += ", type=" + new_hazard.getType();
	      //reportEvent(event);
       }
      }
    }
  
  string summary_report = m_hazard_set.getSpec("final_report");
   
  Notify("HAZARDSET_REPORT", summary_report);
  
  //NodeMessage node_message;

  //node_message.setSourceNode(m_host_community);
  //node_message.setDestNode("all");
  //node_message.setVarName("HAZARDSET_REPORT");
  //node_message.setStringVal(summary_report);
  //what happened to character limit here?
  //string msg = node_message.getSpec();

  //Notify("NODE_MESSAGE_LOCAL", msg); 


 
}

//---------------------------------------------------------
// Procedure: handleMailMissionParams
//   Example: UHZ_MISSION_PARAMS = penalty_missed_hazard=100,               
//                       penalty_nonopt_hazard=55,                
//                       penalty_false_alarm=35,                  
//                       penalty_max_time_over=200,               
//                       penalty_max_time_rate=0.45,              
//                       transit_path_width=25,                           
//                       search_region = pts={-150,-75:-150,-50:40,-50:40,-75}

void HazardMgr::handleMailMissionParams(string str)
{
 string missed =tokStringParse(str,"penalty_missed_hazard",',','=');
  m_missed_hazard = strtod(missed.c_str(),NULL);
  string false_alarm =tokStringParse(str,"penalty_false_alarm",',','=');
  m_false_alarm=strtod(false_alarm.c_str(),NULL);
  string max_time=tokStringParse(str,"max_time",',','=');
  m_max_time=strtod(max_time.c_str(),NULL);
  string time_over=tokStringParse(str,"penalty_max_time_over",',','=');
  m_max_time_over=strtod(time_over.c_str(),NULL);
  string time_rate=tokStringParse(str,"penalty_max_time_rate",',','=');
  m_max_time_rate=strtod(time_rate.c_str(),NULL);
  m_search=tokStringParse(str,"search_region",',','=');
    
  //vector<string> svector = parseStringZ(str, ',', "{");
  //unsigned int i, vsize = svector.size();
  //for(i=0; i<vsize; i++) {
  //  string param = biteStringX(svector[i], '=');
  //  string value = svector[i];
    // This needs to be handled by the developer. Just a placeholder.
  //}
}

//------------------------------------------------------------
// Procedure: buildReport()

bool HazardMgr::buildReport() 
{
  m_msgs << "Config Requested:"                                  << endl;
  m_msgs << "    swath_width_desired: " << m_swath_width_desired << endl;
  m_msgs << "             pd_desired: " << m_pd_desired          << endl;
  m_msgs << "   config requests sent: " << m_sensor_config_reqs  << endl;
  m_msgs << "                  acked: " << m_sensor_config_acks  << endl;
  m_msgs << "------------------------ "                          << endl;
  m_msgs << "Config Result:"                                     << endl;
  m_msgs << "       config confirmed: " << boolToString(m_sensor_config_set) << endl;
  m_msgs << "    swath_width_granted: " << m_swath_width_granted << endl;
  m_msgs << "             pd_granted: " << m_pd_granted          << endl << endl;
  m_msgs << "--------------------------------------------" << endl << endl;

  m_msgs << "               sensor requests: " << m_sensor_report_reqs << endl;
  m_msgs << "             detection reports: " << m_detection_reports  << endl << endl; 
  m_msgs << "             hazard reports: " << m_hazard_reports  << endl << endl; 

  m_msgs << "   Hazardset Reports Requested: " << m_summary_reports << endl;
  m_msgs << "      Hazardset Reports Posted: " << m_summary_reports << endl;
  m_msgs << "                   Report Name: " << m_report_name << endl;
  m_msgs << "                    type test=: " << m_test << endl;
  m_msgs << "                     distance=: " << m_range << endl;
  m_msgs << "                   pd granted=: " << m_pd_granted << endl;
  m_msgs << "                      counter=: " << m_counter2 << endl;
  m_msgs << "                        nav x=: " << m_nav_x << endl;
  

  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailVehicleReport
//   
bool HazardMgr::handleMailVehicleReport(string input)
{

  //parse incoming long report
  
  vector<string> str_vector = parseString(input, "#");
  
  for (unsigned int i=0; i<str_vector.size(); i++)
    {
      string str = str_vector[i];
      
      if (str!="empty")
        handleMailHazardReport(str);
    }  

   return(true);
}

//---------------------------------------------------------
// Procedure: handleMailVehicleReportRequest

void HazardMgr::handleMailVehicleReportRequest()
{
  
  if(m_comms)
    {
      string str;
      unsigned int jj = m_vector_detection.size();
      
      if(jj==0)
	str="empty";
      else
	{
	  
      for (unsigned int i = 0; i < jj && i<2; i++)
        {
	  str = str + m_vector_detection[0] + "#";
          m_vector_detection.erase(m_vector_detection.begin());
        }
	}
      
      NodeMessage node_message;

      node_message.setSourceNode(m_host_community);
      node_message.setDestNode("all");
      node_message.setVarName("VEHICLE_REPORT");
      node_message.setStringVal(str);

      string msg = node_message.getSpec();

      Notify("NODE_MESSAGE_LOCAL", msg);
      m_comm_time=MOOSTime();
    }
}

//---------------------------------------------------------             
// Procedure: handleMailHazardReport                                 
//      Note: UHZ_HAZARD_REPORT should look something like:          
//            UHZ_HAZARD_REPORT = x=185, y=-93, label=64, type=benign   

bool HazardMgr::handleMailHazardReport(string str)
{
  m_hazard_reports++;
  
  XYHazard new_hazard = string2Hazard(str);

  string hazlabel = new_hazard.getLabel();
  
  if(hazlabel == "") {
    reportRunWarning("Detection report received for hazard w/out label"\
		     );
    return(false);
  }

  string haztype = new_hazard.getType();
  
  //if (haztype == "hazard")
  // {
  int ix = m_classified.findHazard(hazlabel);
  if(ix == -1)
    {
      m_new +=1;
      m_classified.addHazard(new_hazard);
    }
  else
    // m_hazard_set.setHazard(ix, new_hazard);
    {
      m_dup +=1;
      XYHazard working = m_classified.getHazard(ix);
      string new_type = working.getType();
      
      new_type = new_type + "#" + haztype;
      
      working.setType(new_type.c_str());
      m_classified.setHazard(ix, working);
    }

  string event = "New Classifcation, label=" + new_hazard.getLabel();
  event += ", x=" + doubleToString(new_hazard.getX(),1);
  event += ", y=" + doubleToString(new_hazard.getY(),1);
  event += ", type=" + new_hazard.getType();
  reportEvent(event);
  // }
  return(true);
}

double HazardMgr::GetDistance(string sval)
{
  
  
  string x = tokStringParse(sval,"x",',','=');
  string y = tokStringParse(sval,"y",',','=');
  double x_val = strtod(x.c_str(),NULL);
  double y_val = strtod(y.c_str(),NULL);
  double range = hypot(m_nav_x-x_val,m_nav_y-y_val);
  return(range);
}

void HazardMgr::EvaluatePD()
{
  m_check = MOOSTime()-m_previous;
  if(m_check>120)
    {
      m_previous=MOOSTime();
      if(m_counter/(m_check/60)>2)
	{
	  m_pd_desired=m_pd_granted-.1;
	  postSensorConfigRequest();
	}
      m_counter=0;
    }
  if(MOOSTime()-m_detection>240)
    {
      m_pd_desired=.7;
      postSensorConfigRequest();
      m_counter=0;
    }
}




