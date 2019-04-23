/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PointAssign.h"
#include <limits>
#include <XYPoint.h>
#include "GeomUtils.h"
#include "ColorPack.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
  m_all_received=false;
  m_all_sent = false;
  m_first_name = true;
  m_assign_by_region= true;
  m_first_ready=true;
  m_second_ready=false;
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
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
    string comm  = msg.GetCommunity();
    //cout << comm.c_str() << endl;
    string key   = msg.GetKey();
    string sval  = msg.GetString();
    if (key=="VEHICLE_READY")
     {
       cout << comm.c_str() << endl;
       if (m_second_ready)
	 {
           m_second_ready=false;
	   Notify("UTS_PAUSE","false");
	 }

       else if (m_first_ready)
	 {
           m_first_ready=false;
	   m_second_ready=true;
	 }
       
     }
    if (key == "VISIT_POINT")
      {
        if (m_all_received!=true)
          {
            if (sval == "firstpoint")
              {
	        m_out1.push_back(sval);
	        m_out2.push_back(sval);
	      }
            else if (sval == "lastpoint")
              {
	        m_out1.push_back(sval);
	        m_out2.push_back(sval);
	        m_all_received = true;
              }
            else if (m_assign_by_region)
              {
                int loc = sval.find(",y");
	        string region = sval.substr(2,loc-2);
	        double region_val = strtod(region.c_str(),NULL);
	        if (region_val >=87.5)
	          m_out1.push_back(sval);
	        else
	          m_out2.push_back(sval);
	      }
	    else
	      {
		int pos = sval.find("id=");
	        string id = sval.substr(pos+3);
                int id_val = std::stoi(id.c_str());
	        if (id_val%2 != 0)
	          m_out1.push_back(sval);
	        else
	          m_out2.push_back(sval);
	      }
	  }
      }
    if (key == "NODE_REPORT")
      {
	string vessel = tokStringParse(sval,"NAME",',','=');
	if (m_first_name)
	  {
	    m_vnames.push_back(vessel.c_str());
	    m_first_name = false;
	  }
        else
	  {
            bool match = false;
	    for (unsigned int kk = 0;kk<m_vnames.size();kk++)
	      {
	        string test = m_vnames[kk];
                if (test == vessel)
	          {
		    match = true;
		  }
		else
		  {}
	      }
	    if (match!=true)
	      {
		m_vnames.push_back(vessel.c_str());
	      }
	    else
	      {}
	 }
      }
  } 	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
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

bool PointAssign::Iterate()
{
  if (m_all_received && (m_all_sent!=true))
    {
      for (unsigned int ii=0;ii<m_vnames.size();ii++)
	{
	  string out_names = m_vnames[ii];
	  out_names = "VISIT_POINT_" + toupper(out_names);
	  m_notify_name.push_back(out_names);
	}
      for (unsigned int ii=0;ii<m_out1.size();ii++)
	{
          string out1_name = m_notify_name[0];
	  string out1_index = m_out1[ii];
	  //cout << out1_name.c_str() << out1_index.c_str() << endl;
	  Notify(out1_name.c_str(), out1_index.c_str());

          if (out1_index!="firstpoint" && out1_index!="lastpoint")
	    {
	  string x = tokStringParse(out1_index,"x",',','=');
	  string y = tokStringParse(out1_index,"y",',','=');
          string label = tokStringParse(out1_index,"id",',','=');
	  
	  double x_val = strtod(x.c_str(),NULL);
	  double y_val = strtod(y.c_str(),NULL);

	  //cout << "x=" << x_val << "y=" << y_val << "label=" << label.c_str() << endl;

	  XYPoint point(x_val, y_val);
          point.set_label(label);
          point.set_color("vertex", "red");
          point.set_param("vertex_size", "5");

          string spec = point.get_spec();
	  //cout << spec.c_str();
          Notify("VIEW_POINT", spec);
            }
	}
      for (unsigned int ii=0;ii<m_out2.size();ii++)
	{
          string out2_name = m_notify_name[1];
	  string out2_index = m_out2[ii];
	  Notify(out2_name.c_str(), out2_index.c_str());
	  
          if (out2_index!="firstpoint" && out2_index!="lastpoint")
	    {
	  string x = tokStringParse(out2_index,"x",',','=');
	  string y = tokStringParse(out2_index,"y",',','=');
          string label = tokStringParse(out2_index,"id",',','=');
	  
	  double x_val = strtod(x.c_str(),NULL);
	  double y_val = strtod(y.c_str(),NULL);

	  XYPoint point(x_val, y_val);
          point.set_label(label);
          point.set_color("vertex", "green");
          point.set_param("vertex_size", "5");

          string spec = point.get_spec();
	  //cout << spec.c_str();
          Notify("VIEW_POINT", spec);
	    }
	} 
      m_all_sent = true;
    }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
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
  
  //RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PointAssign::RegisterVariables()
{
  // Register("FOOBAR", 0);
  Register("VISIT_POINT",0);
  Register("NODE_REPORT",0);
  Register("VEHICLE_READY",0);
  //Notify("UTS_PAUSE","false");
}

