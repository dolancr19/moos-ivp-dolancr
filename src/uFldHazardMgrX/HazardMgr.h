/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardMgr.h                                          */
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

#ifndef UFLD_HAZARD_MGR_HEADER
#define UFLD_HAZARD_MGR_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYHazardSet.h"
#include "XYPolygon.h"
#include <stdlib.h>

using namespace std;

class HazardMgr : public AppCastingMOOSApp
{
 public:
   HazardMgr();
   ~HazardMgr() {}

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected: // Registration, Configuration, Mail handling utils
   void registerVariables();
   bool handleMailSensorConfigAck(std::string);
   bool handleMailSensorOptionsSummary(std::string) {return(true);}
   bool handleMailDetectionReport(std::string);
   bool handleMailHazardReport(std::string);
   void handleMailReportRequest();
   void handleMailMissionParams(std::string);
  //added new function
   bool handleMailVehicleReport(std::string);
   void handleMailVehicleReportRequest();

 protected: 
   void postSensorConfigRequest();
   void postSensorInfoRequest();
   void postHazardSetReport();
   double GetDistance(string);
   void EvaluatePD();
   
 private: // Configuration variables
   double      m_swath_width_desired;
   double      m_pd_desired;
   std::string m_report_name;

 private: // State variables
   bool   m_sensor_config_requested;
   bool   m_sensor_config_set;

   unsigned int m_sensor_config_reqs;
   unsigned int m_sensor_config_acks;

   unsigned int m_sensor_report_reqs;
   unsigned int m_detection_reports;
   unsigned int m_hazard_reports;

   unsigned int m_summary_reports;

   double m_swath_width_granted;
   double m_pd_granted;

   XYHazardSet m_hazard_set;
  XYHazardSet m_detection_set;
  XYHazardSet m_classified;
   XYPolygon   m_search_region;
   
   double      m_transit_path_width;

   bool m_comms;
   double m_previous;
   double m_current_time;
   string m_comm;
   string m_test;
   int m_dup;
   int m_new;

   double m_missed_hazard;
   double m_false_alarm;
   double m_max_time;
   double m_max_time_over;
   double m_max_time_rate;
   string m_search;

   bool m_mstr;
   double m_comm_time;
   double m_comm_request;

   double m_x;
   double m_y;
   double m_nav_x;
   double m_nav_y;

   double m_distance;
   double m_counter;
   double m_detection;
   double m_check;
   double m_range;
   double m_counter2;
   double m_node_time;
   
  //vector to hold incoming reports
  vector<string> m_vector_detection;
};

#endif 








