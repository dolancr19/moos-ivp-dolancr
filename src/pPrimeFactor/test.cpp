#include <iostream>
#include <vector>
#include <cstdlib>
#include <math.h>

using namespace std;

int main ()
{
   uint64_t      m_start_index;
   uint64_t      m_orig = 2310;
   uint64_t      remain;
   bool          m_done;
   unsigned int  m_received_index;
   unsigned int  m_calculated_index;
   uint64_t ii;
   int kk;

   vector<uint64_t> m_factors;

remain = m_orig;
 ii=2;
 while (remain != 1){
   //for (ii=2;ii<=sqrt(m_orig);ii++)
  
   // {
  if (remain % ii == 0)
    {
    remain = remain/ii;
    cout << ii << endl;
    cout << remain << endl;
    m_factors.push_back(ii);
     }
  ii = ii+1;
  // }
 }
cout << "Factors" << endl;
for (kk=0;kk<m_factors.size();kk++)
  {
    cout << m_factors[kk] << endl;
  }
 return(0);
}
   
    
unsigned long int number;
  unsigned long int value;
  string eo;
  stringstream ss;
  string str;
  list<string>::iterator p;
   for(p=my_strings.begin(); p!=my_strings.end(); p++) {
     str = *p;
     uint64_t value = strtoul(str.c_str(),NULL,0);
     if ( value % 2 == 0)
	eo = "even";
	else
	  eo = "odd";
      ss << value << "," << eo;
      Notify("NUM_RESULT", ss.str());
      my_strings.pop_front();
  return(true);
   }
