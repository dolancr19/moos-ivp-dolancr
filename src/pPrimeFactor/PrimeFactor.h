/************************************************************/
/*    NAME: Christopher Dolan                               */
/*    CONTACT: dolancr@mit.edu                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.h                                   */
/*    DATE: 04 March 2019                                   */
/************************************************************/

#ifndef PrimeFactor_HEADER
#define PrimeFactor_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <list>
#include <string>
#include <sstream>
#include "PrimeEntry.h"
#include <iterator>
#include "MBUtils.h"
#include <cstdlib>
#include <math.h> 
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

class PrimeFactor : public CMOOSApp
  {
    public:

      PrimeFactor();
      ~PrimeFactor();

    protected: // Standard MOOSApp functions to overload  
      bool OnNewMail(MOOSMSG_LIST &NewMail);
      bool Iterate();
      bool OnConnectToServer();
      bool OnStartUp();

    protected:
      void RegisterVariables();

    private: // Configuration variables
      unsigned int      ri=1;
      unsigned int      ci=1;
      unsigned long int max_steps=100000;
   
    private: // State variables
      list<PrimeEntry>  primes;
      bool              complete;
      bool              finished;
  };
#endif
