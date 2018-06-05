#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>

class Logger
{
public:
  static const std::string TRACE_LEVEL;
  static const std::string DEBUG_LEVEL;
  static const std::string INFO_LEVEL;
  static const std::string ERROR_LEVEL;
 
  static Logger& instance();

  void log(const std::string& inLogLevel, 
    const std::string& inMessage);

protected:
  static Logger* pInstance;

  static const char* const logFileName;

  std::ofstream logFileOutputStream;

  friend class Cleanup;
  class Cleanup
  {
    public:
      ~Cleanup();
  };

  // Logs message. The thread should own a lock on sMutex
  // before calling this function.
  void logImpl(const std::string& inLogLevel, 
    const std::string& inMessage);

private:
  Logger();
  virtual ~Logger();
  Logger(const Logger&);
  Logger& operator=(const Logger&);
  static std::mutex sMutex;
};
