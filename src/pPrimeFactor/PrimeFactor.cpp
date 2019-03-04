/************************************************************/
/*    NAME: Christopher Dolan                               */
/*    CONTACT: dolancr@mit.edu                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                 */
/*    DATE: 04 March 2019                                   */
/************************************************************/

#include "PrimeFactor.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
  {
    list<PrimeEntry> primes;
    PrimeEntry working;  
    unsigned int ri = 1;
    unsigned int ci = 1;
    unsigned long int max_steps = 100000;
  }

//---------------------------------------------------------
// Destructor

PrimeFactor::~PrimeFactor()
  {
  }

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
  {
    MOOSMSG_LIST::iterator p;
   
    for(p=NewMail.begin(); p!=NewMail.end(); p++)
      {
        CMOOSMsg &msg = *p;
        // Obtain the number to be factored and convert to uint64_t
	string sval  = msg.GetString();
        uint64_t value = strtoul(sval.c_str(),NULL,0);
        // Initialize a new PrimeEntry object
	PrimeEntry working;
        // Set required initial parameters
	working.setOriginalVal(value);
        working.setReceivedIndex(ri);
        working.setStartTime();
        // Place the object in the primes list work queue
	primes.push_back(working);
        // Increment received index in preparation for next mail
	ri+=1;
        return(true);
      }
  }

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactor::OnConnectToServer()
  {
     // register for variables here
   	
     RegisterVariables();
     return(true);
  }

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactor::Iterate()
  {
    // Initialize iterator
    list<PrimeEntry>::iterator p;
    for (p=primes.begin(); p!=primes.end(); p++)
      {
        // Extract an object from the work queue
	PrimeEntry &factoring = *p;
        // Execute factoring function on the extracted object
	complete = factoring.factor(max_steps);
        // Determine if calculation complete
	finished = factoring.done();
        // If the calculation is finished, construct the required output and publish to the MOOSDB
	if (finished)
	  {
	    factoring.setCalculatedIndex(ci);
	    // Increment the calculated index counter
	    ci+=1;
	    string output = factoring.getReport();
            Notify("PRIME_RESULT", output.c_str());
            // Remove the completed object from the work queue
	    p=primes.erase(p);
	  }
      }
    return(true);
  }

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor::OnStartUp()
  {
    list<string> sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    if(m_MissionReader.GetConfiguration(GetAppName(), sParams))
      {
        list<string>::iterator p;
        for(p=sParams.begin(); p!=sParams.end(); p++)
	  {
            string line  = *p;
            string param = tolower(biteStringX(line, '='));
            string value = line;
      
      if(param == "foo")
	{
        //handled
        }
      else if(param == "bar")
	{
        //handled
        }
      }
    }
  
    RegisterVariables();	
    return(true);
  }

//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactor::RegisterVariables()
  {
    // Register for input variable
    
    Register("NUM_VALUE",0);
  }

