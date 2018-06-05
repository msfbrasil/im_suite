#include <stdexcept>
#include <boost/date_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include "logger.h"

using namespace std;

const std::string Logger::TRACE_LEVEL = "TRACE";
const std::string Logger::DEBUG_LEVEL = "DEBUG";
const std::string Logger::INFO_LEVEL = "INFO";
const std::string Logger::ERROR_LEVEL = "ERROR";

//static local_time::time_zone_ptr const utc_time_zone(new local_time::posix_time_zone("GMT"));

const char* const Logger::logFileName = "server.log";

Logger* Logger::pInstance = nullptr;

mutex Logger::sMutex;

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------

Logger& Logger::instance()
{
  static Cleanup cleanup;

  lock_guard<mutex> guard(sMutex);
  if (pInstance == nullptr)
  {
    pInstance = new Logger();
  }
  return *pInstance;
}

Logger::Cleanup::~Cleanup()
{
  lock_guard<mutex> guard(Logger::sMutex);
  delete Logger::pInstance;
  Logger::pInstance = nullptr;
}

Logger::~Logger()
{
  logFileOutputStream.close();
}

Logger::Logger()
{
  logFileOutputStream.open(logFileName, ios_base::app);
  if (!logFileOutputStream.good())
  {
    throw runtime_error("Unable to initialize the Logger!");
  } 
}

//----------------------------------------------------------------------
// Public methods.
//----------------------------------------------------------------------

void Logger::log(const string& inLogLevel, const string& inMessage)
{
  lock_guard<mutex> guard(sMutex);
  logImpl(inLogLevel, inMessage);
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

void Logger::logImpl(const std::string& inLogLevel, const std::string& inMessage)
{
  //boost::posix_time::time_facet tf(1);
  //tf.set_iso_extended_format();
  //locale loc = locale(locale("pt_BR"), tf);
  //cout.imbue(loc)

  logFileOutputStream << inLogLevel << " -> " << inMessage << endl;
}

