/************************************************************/
/*    NAME: Christopher Dolan                               */
/*    CONTACT: dolancr@mit.edu                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.cpp                                  */
/*    DATE: 04 March 2019                                   */
/************************************************************/

#include "PrimeEntry.h"
#include <string>
#include <sstream>
#include <vector> 
#include <cstdint>
#include "MOOS/libMOOS/MOOSLib.h"
#include "MBUtils.h"
#include <iostream>
using namespace std;

//---------------------------------------------------------
// Constructor

PrimeEntry::PrimeEntry()
  {
    m_start_index=3;
    m_done=false;
  }

//---------------------------------------------------------
// Destructor

PrimeEntry::~PrimeEntry()
  {
  }


void PrimeEntry::setOriginalVal(uint64_t value)
  {
    m_orig = value;
    m_remain = value;
  }

void PrimeEntry::setReceivedIndex(unsigned int m_ri)
  {
    m_received_index = m_ri;
  }

void PrimeEntry::setCalculatedIndex(unsigned int m_ci)
  {
    m_calculated_index = m_ci;
  }

bool PrimeEntry::done()
  {
    return(m_done);
  }

void PrimeEntry::setStartTime()
  {
    m_start_time = MOOSTime();
  }

bool PrimeEntry::factor(unsigned long int m_max_steps)
  {
    // Set or reset the steps counter to 1
    m_current_steps=1;
    // Calculate factors of 2
    while (m_remain%2 == 0)
      {
        m_factors.push_back(2);
        m_remain = m_remain/2;
      }
    // The remainder must be odd, so the for loop can increment by 2 each step
    // The for loop logic prevents iterating past the square root of the original number or the max allowed steps
    for (int jj = m_start_index; (jj<=sqrt(m_orig)) && (m_current_steps<=m_max_steps); jj+=2)
      {
        // The while loop ensures a factor that divides more than once will be properly accounted for
	while (m_remain % m_start_index == 0)
          { 
            m_factors.push_back(m_start_index);
	    m_remain = m_remain/m_start_index;
	  }
	// Iterate counters.  The start index will be used to restart calculations at the proper factor for the next loop
        m_current_steps+=1;
        m_start_index+=2;
      }   
    // If the loop completed without reachin max allowed steps, the calculation is complete
    if (m_current_steps <= m_max_steps)
      {
        // Ensure all factors are accounted for
	if (m_remain != 1)
          {
            m_factors.push_back(m_remain);
          }
        // Calculate calculation time
	m_stop_time = MOOSTime();
        m_calc_time = m_stop_time-m_start_time;
        // Set logic flag to true to allow for publishing in subsequent function
	m_done=true;
	return(true);
      }
    // Reports completion of the function, but does not change logic flag
    return(true);
  }

string PrimeEntry::getReport()
  {
    // Generate a string for the required output
    stringstream ss;
    ss << "orig=" << m_orig << ",received=" << m_received_index << ",calculated=" << m_calculated_index << ",solve_time=" << m_calc_time << ",primes=" << m_factors[0];
    for (unsigned int jj=1;jj<m_factors.size();jj++)
      {
        ss << ":" << m_factors[jj];
      }
    ss << ",username=dolancr";
    return(ss.str());
  }
