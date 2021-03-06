/************************************************************/
/*    NAME: Christopher Dolan                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Pulse.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_Pulse.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_Pulse::BHV_Pulse(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y");
  addInfoVars("WPT_INDEX","no_warning");

  m_wpt_index=0;
  m_osx=0;
  m_osy=0;
  m_rad=0;
  m_curr_time=0;
  m_pulse_duration=0;
  m_post_test=false;
  m_post_time=0;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_Pulse::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if(param=="pulse_range")
    m_rad=double_val;
  else if(param=="pulse_duration")
    m_pulse_duration=double_val;
  else

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_Pulse::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_Pulse::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_Pulse::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_Pulse::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_Pulse::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_Pulse::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_Pulse::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_Pulse::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;

  bool ok1, ok2, ok3;
  m_osx=getBufferDoubleVal("NAV_X",ok1);
  m_osy=getBufferDoubleVal("NAV_Y",ok2);
  if(!ok1 || !ok2)
    {
      postWMessage("No ownship X/Y in buffer.");
      return(0);
    }
  double new_index=getBufferDoubleVal("WPT_INDEX",ok3);
  m_curr_time=getBufferCurrTime();
  if(ok3)
    {
      if(new_index!=m_wpt_index)
	{
	  m_post_time=m_curr_time+5;
	  m_post_test=true;
	  m_wpt_index=new_index;
	}
      if(m_post_test)
	{
	  if(m_curr_time>=m_post_time)
	    {
	      XYRangePulse pulse;
              pulse.set_x(m_osx);
              pulse.set_y(m_osy);
              pulse.set_label("bhv_pulse");
              pulse.set_rad(m_rad);
              pulse.set_duration(m_pulse_duration);
              pulse.set_time(m_curr_time);
              pulse.set_color("edge", "yellow");
              pulse.set_color("fill", "yellow");

              string spec = pulse.get_spec();
              postMessage("VIEW_RANGE_PULSE", spec);
	      m_post_test=false;
	    }
	  
	}
    }

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

