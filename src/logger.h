//
// logger.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#ifndef IM_LOGGER_H
#define IM_LOGGER_H

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

//----------------------------------------------------------------------
// LogEntry
//----------------------------------------------------------------------

class LogEntry
{
  public:
    LogEntry(const std::string& logLevel, 
      const std::string& message) : 
      logLevel_(logLevel),
      message_(message)
    {
    }

    std::string getLogLevel() const
    {
      return logLevel_;
    }

    std::string getMessage() const
    {
      return message_;
    }
  
  private:
    std::string logLevel_;
    std::string message_;
};

typedef std::shared_ptr<LogEntry> LogEntryPtr;

//----------------------------------------------------------------------
// LogWorker
//----------------------------------------------------------------------

class LogWorker
{
  public:
    LogWorker();
    ~LogWorker();

    void start();
    void stop();
    void enqueueLog( LogEntryPtr pLogEntry );

  private:
    static const char* const m_logFileName;
    std::ofstream m_logFileOutputStream;
    bool m_workerIsDone;
    boost::mutex m_workerMutex;
    boost::condition_variable m_conditionVariable;
    std::deque<LogEntryPtr> m_logQueue;
};

//----------------------------------------------------------------------
// Logger
//----------------------------------------------------------------------

class Logger
{
public:
  static const std::string TRACE_LEVEL;
  static const std::string DEBUG_LEVEL;
  static const std::string INFO_LEVEL;
  static const std::string ERROR_LEVEL;

  static Logger& instance();

  void log(const std::string& logLevel, 
    const std::string& inMessage);

protected:
  static Logger* m_pInstance;

  // Embedded class to make sure the single Logger
  // instance gets deleted on program shutdown.
  friend class Cleanup;
  class Cleanup
  {
    public:
      ~Cleanup();
  };

  // Enqueues messages on the worker thread.
  //
  void logImpl(const std::string& logLevel, 
    const std::string& inMessage);

private:
  Logger();
  virtual ~Logger();
  Logger(const Logger&);
  Logger& operator=(const Logger&);
  static boost::mutex sMutex;
  LogWorker m_logWorker;
  boost::thread m_logWorkerThread;
};

#endif // IM_LOGGER_H

