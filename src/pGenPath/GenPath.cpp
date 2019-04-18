/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GenPath.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
  m_first=true;
  m_first_x=true;
  m_first_y=true;
  m_first_range=true;
  m_all_received=false;
  m_path=true;
  m_notify=true;
  m_regen=false;
  m_dist=0;
  m_inserted=false;
  m_dist_test=0;
  m_nav_x=0;
  m_nav_y=0;
  m_x=0;
  m_y=0;
  m_last_range=0;
  m_wpt=0;
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
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
   string sval  = msg.GetString();
   double dval  = msg.GetDouble();
   if (key == "GENPATH_REGENERATE")
     {
       m_regen = sval.c_str();
       cout << " mail regen " << m_regen << endl;
     }
   

   if (key == "NAV_X")
     {
       //if (m_first_x)
       //{
	   m_nav_x=dval;
           //m_first_x=false;
	   //}
     }
   if (key == "NAV_Y")
     {
       //if (m_first_y)
       //{
	   m_nav_y=dval;
           //m_first_y=false;
	   //}
     }
   if (key == "VISIT_POINT")
     {
       if (m_all_received!=true)
	 {
	   if (sval!="firstpoint" && sval!="lastpoint")
             {
               m_points.push_back(sval);
	     }
		     
	    else if (sval=="lastpoint")
	     m_all_received=true;
	   
	 }
     }
  }
  
  return(true);
}
  
     
     

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenPath::OnConnectToServer()
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

bool GenPath::Iterate()
{
  if(m_notify)
    {
      Notify("VEHICLE_READY","true");
      m_notify=false;
    }
  
  if (m_all_received && m_path)
    {
      m_x=m_nav_x;
      m_y=m_nav_y;
      while(m_points.size()>1)
	{
	  bool first_string=true;
	  string sorting = SortStrings(first_string);
	  m_route.push_back(sorting);
	  string get_x = tokStringParse(sorting,"x",',','=');
	  string get_y = tokStringParse(sorting,"y",',','=');
	  m_x = strtod(get_x.c_str(),NULL);
	  m_y = strtod(get_y.c_str(),NULL);
	  RemoveString(sorting);
	}
      //cout<< update_str.c_str() << endl;
      m_route.push_back(m_points[0]);
      //cout << m_route.size() << endl;
      m_path=false;
      string update = GenOutput();
      Notify("TRANSIT_UPDATES", update);
      Notify("DEPLOY","true");
      Notify("MOOS_MANUAL_OVERRIDE","false");
      Notify("RETURN","false");
      Notify("STATION_KEEP","false");
      //Notify("REFUELING","true");
    }

  if (m_all_received && m_path!=true)
    {
      if (m_wpt<m_route.size())
      	{
          double visit_radius = 3;
	  string distance = m_route[m_wpt];
          double range = GetDistance(distance);
	  if(m_first_range)
	    {
	      m_last_range=range;
	      m_first_range=false;
	    }
	  if (range<10)
	    {
	      if (range<visit_radius)
		{
		  m_wpt+=1;
		  m_last_range=range;
		}
	      else if (range>m_last_range)
		{
		  m_missed_points.push_back(distance);
		  m_wpt+=1;
		  m_last_range=range;
		}
	    }
	    else
	      {
		m_last_range=range;
	      }
	    }
    }

  if (m_regen)
    {
      cout << "missed points " << m_missed_points.size() << endl;
      //cout << "m regen " << m_regen << endl;
      if (m_missed_points.size()>0)
	{
	  m_points.clear();
	  m_route.clear();
	  m_points_sorted.clear();
	  //cout << "m points " << m_points.size() << endl;
	  //cout << "m route " << m_route.size() << endl;
	  //cout << "m points sorted " << m_points.size() << endl;
          for (unsigned int ii =0;ii<m_missed_points.size();ii++)
	    {
	      string regen_point = m_missed_points[ii];
	      m_points.push_back(regen_point);
	    }
      
      //cout << "m points new size " << m_points.size() << endl;
     

      //jj=m_points.begin();
      //m_points.insert(jj,m_missed_points.begin(),m_missed_points.end());
          m_missed_points.clear();
	  m_path=true;
          m_regen=false;
	  Notify("REFUELING", "true");
      //Notify("GENPATH_REGENERATE","false");
      //cout << "missed " << m_missed_points.size() << endl;
	}
      else
	{
	  m_path=false;
	  m_regen=false;
	  Notify("STATION_KEEPING","true");
	}
    }
	      
  return(true);
}

double GenPath::GetDistance(string distance)
{
  
  
  string x = tokStringParse(distance,"x",',','=');
  string y = tokStringParse(distance,"y",',','=');
  double x_val = strtod(x.c_str(),NULL);
  double y_val = strtod(y.c_str(),NULL);
  double range = hypot(m_nav_x-x_val,m_nav_y-y_val);
  return(range);
}

string GenPath::SortStrings(bool first_string)
{
  vector<string>::iterator it;
      for (it=m_points.begin();it!=m_points.end();it++)
	{
          string testing = *it;
	  if (first_string)
	    {
	      m_points_sorted.push_back(testing);
	      first_string=false;
	      //cout << "first" << endl;
	    }
	  else
	    {
              //cout << "second" << endl;
	      string x_test = tokStringParse(testing,"x",',','=');
	      string y_test = tokStringParse(testing,"y",',','=');
              double x_test_val = strtod(x_test.c_str(),NULL);
	      double y_test_val = strtod(y_test.c_str(),NULL);
	      m_dist_test = hypot(m_x-x_test_val,m_y-y_test_val);
		//sqrt(pow(m_x-x_test_val,2)+pow(m_y-y_test_val,2));
	      vector<string>::iterator itt;
	      m_inserted = false;
	      for (itt=m_points_sorted.begin(); (itt!=m_points_sorted.end()) && (m_inserted!=true);itt++)
	        {
                  string sorting = *itt;
		  string x_test1 = tokStringParse(sorting,"x",',','=');
                  string y_test1 = tokStringParse(sorting,"y",',','=');
                  double x_test_val1 = strtod(x_test1.c_str(),NULL);
	          double y_test_val1 = strtod(y_test1.c_str(),NULL);
	          m_dist = hypot(m_x-x_test_val1,m_y-y_test_val1);
		    //sqrt(pow(40-x_test_val1,2)+pow(y_test_val1,2));
                  //cout << "m dist= "<<m_dist << endl;
		  //cout <<"m dist test= "<< m_dist_test << endl;
		  bool truth = isless(m_dist_test,m_dist);
		  if (truth)
		    {
                      //cout << "truth" << endl;
		      m_points_sorted.insert(itt, testing);
		      //cout << "inserted" << endl;
		      m_inserted=true;
		    }
		}
		if (m_inserted!=true)
		  {
	            m_points_sorted.push_back(testing);
		    //cout << "pushed back" << endl;
	            //m_inserted=false;
	          }
	    }
	}
      string closest = m_points_sorted[0];
      //m_points_sorted.clear();
      //it=m_points.erase(it);
      return(closest);
}

void GenPath::RemoveString(string sorting)
{
  vector<string>::iterator it;
  bool compare_needed=true;
  for (it=m_points.begin();(it!=m_points.end()) && (compare_needed);it++)
    {
      string compare = *it;
      if (compare==sorting)
	{
	  it=m_points.erase(it);
	  m_points_sorted.clear();
	  compare_needed = false;
	}
    }
}
string GenPath::GenOutput()
{
  XYSegList my_seglist;
  for (unsigned int ii=0;ii<m_route.size();ii++)
	{
          string working = m_route[ii];
	  string x = tokStringParse(working,"x",',','=');
          string y = tokStringParse(working,"y",',','=');
          string label = tokStringParse(working,"id",',','=');
	  
          double x_val = strtod(x.c_str(),NULL);
          double y_val = strtod(y.c_str(),NULL);

          
          my_seglist.add_vertex(x_val, y_val);
	}
      string update_str = "points = ";
      update_str += my_seglist.get_spec();
      return(update_str);
}
  
//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
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

void GenPath::RegisterVariables()
{
  // Register("FOOBAR", 0);
 
      Register("VISIT_POINT",0);
      Register("NAV_X",0);
      Register("NAV_Y",0);
      Register("GENPATH_REGENERATE",0);
      //cout << "Unpaused" << endl;
    
}

