/****************************************************************************
  FileName     [ myUsage.h ]
  PackageName  [ util ]
  Synopsis     [ Report the run time and memory usage ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2010 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_USAGE_H
#define MY_USAGE_H

#include <sys/times.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
using namespace std;


#undef MYCLK_TCK
#define MYCLK_TCK sysconf(_SC_CLK_TCK)


class MyUsage
{
public:
   MyUsage() { reset(); }

   void reset() {
      _initMem = checkMem();
      _initPow = checkPower();
      _currentTick =  checkTick();
      _periodUsedTime = _totalUsedTime = 0.0;
   }

   void report(bool repTime, bool repMem, bool repPow) {
       cout<<"****************************************"<<endl;
      if (repTime) {
         setTimeUsage();
         /*cout << "Period time used : " << setprecision(4)
              << _periodUsedTime << " seconds" << endl;*/
         cerr << "Total time used  : " << setprecision(4)
              << _totalUsedTime << " seconds" << endl;
      }
      if (repMem) {
         setMemUsage();
         cerr << "Total memory used: " << setprecision(4)
              << _currentMem << " M Bytes" << endl;
      }
      if (repPow) {
	 setPowerUsage();
	 cerr << "Total power used: " << _currentPow << " mW" << endl;
      }
       cerr<<"****************************************"<<endl;
   }

private:
   // for Memory usage
   double     _initMem;
   double     _currentMem;

   // for Power usage
   int	      _initPow;
   int	      _currentPow;
   

   // for CPU time usage
   double     _currentTick;
   double     _periodUsedTime;
   double     _totalUsedTime;

   // private functions
   double checkMem() const {
      ifstream inf("/proc/self/status");
      if (!inf) {
         cerr << "Cannot get memory usage" << endl;
         return 0.0;
      }
      const size_t bufSize = 128;
      char bufStr[bufSize];
      while (!inf.eof()) {
         inf.getline(bufStr, bufSize);
         if (strncmp(bufStr, "VmSize", 6) == 0) {
            long memSizeLong = atol(strchr(bufStr, ' '));
            return (memSizeLong / 1024.0);
         }
      }
      return 0.0;
   }
   int checkPower() const{
    ifstream inf("/proc/acpi/battery/BAT0/state");
    if(!inf){
	cerr << "Cannot get power state" << endl;
	return 0;
    }	
    const size_t bufSize = 128;
    char bufStr[bufSize];
    while(!inf.eof()){
	inf.getline(bufStr,bufSize);
	if(strncmp(bufStr,"remaining capacity",18)==0){
	    long power = atol(strchr(bufStr,':')+1);
	    return (int)power;
	}
    }
    return 0;
   } 
   double checkTick() const {
      tms tBuffer;
      times(&tBuffer);
      return tBuffer.tms_utime;
   }
   void setMemUsage() { _currentMem = checkMem() - _initMem; }
   void setPowerUsage() { _currentPow = -checkPower() + _initPow ;}
   void setTimeUsage() {
      double thisTick = checkTick();
      _periodUsedTime = (thisTick - _currentTick) / double(MYCLK_TCK);
      _totalUsedTime += _periodUsedTime;
      _currentTick = thisTick;
   }
      
};

#endif // MY_USAGE_H
