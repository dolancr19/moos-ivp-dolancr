/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <vector>
#include <iostream>
#include "XYSegList.h"
#include <string>

using namespace std;

class GenPath : public CMOOSApp
{
 public:
   GenPath();
   ~GenPath();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();
   string SortStrings(bool first_string);
   void RemoveString(string sorting);
   string GenOutput();
   double GetDistance(string distance);

 private: // Configuration variables
   bool m_first;
   bool m_first_x;
   bool m_first_y;
   bool m_all_received;
   bool m_path;
   bool m_inserted;
   bool m_notify;
   bool m_first_range;
   bool m_regen;
 private: // State variables
   vector<string> m_points;
   vector<string> m_points_sorted;
   vector<string> m_route;
   vector<string> m_missed_points;
   //XYSegList m_my_seglist;
   double m_dist;
   double m_dist_test;
   double m_nav_x;
   double m_nav_y;
   double m_x;
   double m_y;
   double m_last_range;
   unsigned int m_wpt;
};

#endif 
