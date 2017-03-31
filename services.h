//--------------------------------------------------------------- -*- c++ -*- --
/// @addtogroup osi

///
/// @file   services.h
/// @since  Fev, 11, 2015
/// @brief  General services.
///
/// @author claudio.acsilva@gmail.com
///

#ifndef OSI_SERVICES_H_INCLUDED
#define OSI_SERVICES_H_INCLUDED

#include <fstream>
#include <string>

namespace osi {
  template<class T> class Singleton 
  {
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);

  protected:
    Singleton() {}
    virtual ~Singleton() {}

  public:
    static T& instance() 
    {
      static T theInstance;
      return theInstance;
    }
  };

  class LogFile : public Singleton<LogFile> 
  {
    std::ofstream log;

  protected:
    friend class Singleton<LogFile>;
    LogFile(): log() {}

  public:
    void open(std::string &name);
    void close();
    bool is_open();
    std::ofstream& logfile();
  };
}

#endif // OSI_SERVICES_H_INCLUDED
//
//-- services.h ----------------------------------------------------------------
