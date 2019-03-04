/************************************************************/
/*    NAME: Christopher Dolan                               */
/*    CONTACT: dolancr@mit.edu                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.h                                    */
/*    DATE: 04 March 2019                                   */
/************************************************************/

#ifndef PRIME_ENTRY_HEADER
#define PRIME_ENTRY_HEADER

#include <string>
#include <sstream>
#include <vector> 
#include <cstdint>
#include "MOOS/libMOOS/MOOSLib.h"
#include "MBUtils.h"
#include <iostream>

using namespace std;

class PrimeEntry
  {
    public:

      PrimeEntry(){};
      ~PrimeEntry(){};

      void setOriginalVal(unsigned long int value);
      void setReceivedIndex(unsigned int ri);
      void setStartTime();
      void setCalculatedIndex(unsigned int ci);  
      bool done();
      bool factor(unsigned long int max_steps);
      string  getReport();

  private: // Configuration variables
      uint64_t          m_start_index=3;
      bool              m_done=false;

  private: // State variables
      uint64_t          m_orig;
      uint64_t          m_remain;
      unsigned int      m_received_index;
      unsigned int      m_calculated_index;
      double            m_start_time;
      double            m_stop_time;
      double            m_calc_time;
      unsigned long int m_current_steps;
      vector<uint64_t>  m_factors;
   };
#endif 
