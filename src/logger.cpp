//
// logger.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#include <stdexcept>
#include <boost/date_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include "logger.h"

using namespace std;

//----------------------------------------------------------------------
// LogWorker
//----------------------------------------------------------------------

const char* const LogWorker::m_logFileName = "server.log";

LogWorker::LogWorker() : 
  m_workerIsDone( false )
{
  m_logFileOutputStream.open(m_logFileName, std::ios_base::app);
  if (!m_logFileOutputStream.good())
  {
    throw std::runtime_error("Unable to initialize the Logger!");
  } 
}

LogWorker::~LogWorker()
{
  m_logFileOutputStream.close();
}

void LogWorker::start()
{
  boost::unique_lock<boost::mutex> scoped_lock( m_workerMutex );
  while ( !m_workerIsDone )
  {
    std::cout << "Waiting for a message to log...\n";
    m_conditionVariable.wait(scoped_lock);
    for ( auto logEntry : m_logQueue )
    {
      std::cout << "Processing message...\n";
      m_logFileOutputStream << logEntry->getLogLevel() 
        << " -> " << logEntry->getMessage() << std::endl;
    }
    m_logQueue.clear();
  }
}

void LogWorker::stop()
{
  boost::unique_lock<boost::mutex> scoped_lock( m_workerMutex );
  m_workerIsDone = true;
  m_conditionVariable.notify_one();
}

void LogWorker::enqueueLog( LogEntryPtr pLogEntry )
{
  boost::unique_lock<boost::mutex> scoped_lock( m_workerMutex );
  m_logQueue.push_back( pLogEntry );
  m_conditionVariable.notify_one();
}


//----------------------------------------------------------------------
// Logger
//----------------------------------------------------------------------


const std::string Logger::TRACE_LEVEL = "TRACE";
const std::string Logger::DEBUG_LEVEL = "DEBUG";
const std::string Logger::INFO_LEVEL = "INFO";
const std::string Logger::ERROR_LEVEL = "ERROR";

Logger* Logger::m_pInstance = nullptr;

boost::mutex Logger::sMutex;

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------

Logger& Logger::instance()
{
  static Cleanup cleanup;

  boost::unique_lock<boost::mutex> scoped_lock( sMutex );
  if (m_pInstance == nullptr)
  {
    m_pInstance = new Logger();
  }

  m_pInstance->m_logWorkerThread = 
    boost::thread([&](){ m_pInstance->m_logWorker.start(); });
  
  return *m_pInstance;
}

Logger::Cleanup::~Cleanup()
{
  boost::unique_lock<boost::mutex> scoped_lock( sMutex );
  m_pInstance->m_logWorker.stop();
  m_pInstance->m_logWorkerThread.join();
  delete Logger::m_pInstance;
  Logger::m_pInstance = nullptr;
}

Logger::~Logger()
{
}

Logger::Logger()
{
}

//----------------------------------------------------------------------
// Public methods.
//----------------------------------------------------------------------

void Logger::log(const string& logLevel, const string& message)
{
  boost::unique_lock<boost::mutex> scoped_lock( sMutex );
  logImpl(logLevel, message);
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

void Logger::logImpl(const std::string& logLevel, const std::string& message)
{
  //boost::posix_time::time_facet tf(1);
  //tf.set_iso_extended_format();
  //locale loc = locale(locale("pt_BR"), tf);
  //cout.imbue(loc)
  m_pInstance->m_logWorker.enqueueLog( 
    std::make_shared<LogEntry>( logLevel, message ) );
}

