#include<services.h>

namespace osi {
  void LogFile::open(std::string &name) { log.open(name.c_str()); }
  
  std::ofstream& LogFile::logfile(){ return log; }
  
  void LogFile::close() { log.close(); }
   
  bool LogFile::is_open() { return log.is_open(); }
}
