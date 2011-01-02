/*
 * processhandler.cpp
 *
 *  Created on: Jun 3, 2010
 *      Author: sueh
 */
#include <processhandler.h>
#include <QTextStream>
#include <QDir>
#include <parse_params.h>
#include <QFile>
#include "b3dutil.h"
#include <typeinfo>

const static QString pidTag = "PID:";

ProcessHandler::ProcessHandler() {
  mJobFile = NULL;
  mQidFile = NULL;
  mProcesschunks = NULL;
  mComFileJobIndex = -1;
  mMachine = NULL;
  mProcess = NULL;
  mKillProcess = new QProcess(this);
  mKillProcess->setProcessChannelMode(QProcess::ForwardedChannels);
  QObject::connect(mKillProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
      SLOT(handleKillFinished(int, QProcess::ExitStatus)));
  mDecoratedClassName = typeid(*this).name();
  mJobFileTextStream = NULL;
  mLogFile = NULL;
  mComFileJob = NULL;
  mQidFileTextStream = NULL;
  resetFields();
}

void ProcessHandler::resetFields() {
  mLogFileExists = false;
  mErrorSignalReceived = false;
  mProcessError = -1;
  mStartedSignalReceived = false;
  mFinishedSignalReceived = false;
  mPidTimerId = 0;
  mStartTime.start();
  mStartingProcess = false;
  mRanContinueKillProcess = false;
  mKillFinishedSignalReceived = false;
  mKill = false;
  resetSignalValues();
  mPausing = 0;
  mStderr.clear();
  mPid.clear();
  mLocalKill = false;
}

ProcessHandler::~ProcessHandler() {
  delete mKillProcess;
  delete mProcess;
  delete mLogFile;
  delete mJobFile;
  delete mJobFileTextStream;
  delete mQidFile;
  delete mQidFileTextStream;
}

void ProcessHandler::initProcess() {
  if (mProcess != NULL) {
    disconnect(mProcess, SIGNAL(error(QProcess::ProcessError)), this,
        SLOT(handleError(QProcess::ProcessError)));
    disconnect(mProcess, SIGNAL(started()), this, SLOT(handleStarted()));
    disconnect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
        SLOT(handleFinished(int, QProcess::ExitStatus)));
    disconnect(mProcess, SIGNAL(readyReadStandardError()), this,
        SLOT(handleReadyReadStandardError()));
    disconnect(mKillProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
        SLOT(handleKillFinished(int, QProcess::ExitStatus)));
    delete mProcess;
    mProcess = NULL;
  }
  mProcess = new QProcess(this);
  //mProcess->setProcessChannelMode(QProcess::MergedChannels);
  QObject::connect(mProcess, SIGNAL(error(QProcess::ProcessError)),
      SLOT(handleError(QProcess::ProcessError)));
  QObject::connect(mProcess, SIGNAL(started()), SLOT(handleStarted()));
  QObject::connect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
      SLOT(handleFinished(int, QProcess::ExitStatus)));
  QObject::connect(mProcess, SIGNAL(readyReadStandardError()),
      SLOT(handleReadyReadStandardError()));
}

//Set mFlag to -1 for sync com files.
//Return true for non-sync files.
void ProcessHandler::setup(Processchunks &processchunks) {
  mProcesschunks = &processchunks;
  mEscapedRemoteDirPath = mProcesschunks->getRemoteDir();
  mEscapedRemoteDirPath.replace(QRegExp(" "), "\\ ");
  if (mProcesschunks->isQueue()) {
    //Queue command
    //finishes after putting things into the queue
    //$queuecom -w "$curdir" -a R $comname:r
    mCommand = mProcesschunks->getQueueCommand();
  }
  else {
    //Local host command
    //csh -ef < $cshname >& $pidname ; \rm -f $cshname &
#ifndef _WIN32
    mCommand = "csh";
#else
    mCommand = "tcsh";
#endif
  }
  initProcess();
}

void ProcessHandler::setJob(ComFileJob &comFileJob, const int jobIndex) {
  if (mComFileJobIndex != -1) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__
        << ":Unable is set job, process handler contains a runnable job:"
        << mComFileJob->getComFileName() << ",mComFileJobIndex:"
        << mComFileJobIndex << endl;
    return;
  }
  resetFields();
  mComFileJob = &comFileJob;
  mComFileJobIndex = jobIndex;
  delete mLogFile;
  mLogFile = NULL;
  mLogFile = new QFile(mComFileJob->getLogFileName());
  if (mProcesschunks->isQueue()) {
    delete mJobFile;
    mJobFile = NULL;
    mJobFile = new QFile(mComFileJob->getJobFileName());
    delete mJobFileTextStream;
    mJobFileTextStream = NULL;
    mJobFileTextStream = new QTextStream(mJobFile);
    delete mQidFile;
    mQidFile = NULL;
    mQidFile = new QFile(mComFileJob->getQidFileName());
    delete mQidFileTextStream;
    mQidFileTextStream = NULL;
    mQidFileTextStream = new QTextStream(mQidFile);
    mParamList << mProcesschunks->getQueueParamList() << "-w"
        << mEscapedRemoteDirPath << "-a" << "R" << mComFileJob->getRoot();
  }
  else {
    mParamList.clear();
    mParamList << "-ef" << mComFileJob->getCshFileName();
  }
}

//Prevent job from being run, but still allow cleanup.
void ProcessHandler::invalidateJob() {
  mComFileJobIndex = -1;
}

//Returns true if job is runnable using this process handler.
const bool ProcessHandler::isJobValid() {
  return mComFileJobIndex != -1;
}

const int ProcessHandler::getAssignedJobIndex() {
  return mComFileJobIndex;
}

void ProcessHandler::setFlagNotDone(const bool singleFile) {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  mComFileJob->setFlagNotDone(singleFile);
}

void ProcessHandler::resetSignalValues() {
  mFinishedSignalReceived = false;
  mErrorSignalReceived = false;
  mStartedSignalReceived = false;
  mExitCode = -1;
  mExitStatus = -1;
  mProcessError = -1;
}

const ComFileJob::FlagType ProcessHandler::getFlag() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return ComFileJob::done;
  }
  return mComFileJob->getFlag();
}

const bool ProcessHandler::logFileExists(const bool newlyCreatedFile) {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return false;
  }
  if (mLogFileExists) {
    return true;
  }
  if (!newlyCreatedFile) {
    //Don't set mLogFileExists when newlyCreateFile is false because ls may need
    //to be run (with handleFileSystemBug) later - the file may be backed up.

    return mProcesschunks->getCurrentDir().exists(mLogFile->fileName());
  }
  //Set mLogFileExists.  Run ls (with handleFileSystemBug) if necessary.
  mLogFileExists = mProcesschunks->getCurrentDir().exists(mLogFile->fileName());
  if (!mLogFileExists && newlyCreatedFile) {
    mProcesschunks->handleFileSystemBug();
    mLogFileExists = mProcesschunks->getCurrentDir().exists(
        mLogFile->fileName());
  }
  return mLogFileExists;
}

const bool ProcessHandler::qidFileExists() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return false;
  }
  if (mProcesschunks->isQueue()) {
    return mProcesschunks->getCurrentDir().exists(mQidFile->fileName());
  }
  return true;
}

//Looks for PID in either stderr (non-queue) or in .qid file (queue).
const QString ProcessHandler::getPid() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return "";
  }
  if (!mProcesschunks->isQueue()) {
    readAllStandardError();
    QTextStream textStream(mStderr);
    getPid(textStream, true);
  }
  else {
    if (!mQidFile->open(QIODevice::ReadOnly)) {
      return mPid;
    }
    getPid(*mQidFileTextStream, true);
  }
  return mPid;
}

/**
 * Returns true if there is a pid in stderr.  Always returns fase if queue is
 * set.
 */
const bool ProcessHandler::isPidInStderr() {
  if (mProcesschunks->isQueue()) {
    return false;
  }
  readAllStandardError();
  QTextStream textStream(mStderr);
  return getPid(textStream, false);
}

/**
 * return true if the pid is found in stream.  If save is true, save the pid
 * to mPid; in this case only check for the pid if mPid is empty.
 */
const bool ProcessHandler::getPid(QTextStream &stream, const bool save) {
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":mPid:" << mPid << endl;
  }
  if (save && !mPid.isEmpty()) {
    //Don't look for the PID more then once
    return true;
  }
  //Don't set the PID unless the line is conplete (includes an EOL).  Process
  //may not be finished when this function runs.
  QString output = stream.readAll();
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":getPid:output:" << output << endl;
  }
  //Look for a PID entry with an EOL so the the complete PID is collected
  int index = output.lastIndexOf("PID:");
  if (index != -1) {
    int endIndex;
#ifdef _WIN32
    endIndex = output.indexOf("\r\n", index);
#else
    endIndex = output.indexOf('\n', index);
#endif
    if (endIndex != -1) {
      if (save) {
        mPid = output.mid(index + 4, endIndex - (index + 4));
      }
      return true;
    }
  }
  return false;
}

const QByteArray ProcessHandler::readAllLogFile() {
  QByteArray log;
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return log;
  }
  if (!mLogFile->open(QIODevice::ReadOnly)) {
    return log;
  }
  log = mLogFile->readAll();
  mLogFile->close();
  return log;
}

const bool ProcessHandler::isLogFileEmpty() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return true;
  }
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":isLogFileEmpty:size:" << mLogFile->size() << endl;
  }
  return mLogFile->size() == 0;
}

void ProcessHandler::readAllStandardError() {
  QByteArray err = mProcess->readAllStandardError();
  if (!err.isEmpty()) {
    mStderr.append(err);
    //if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    printf("stderr:%s\n", err.data());
    //}
  }
}

//Looks for cd or ssh error in either stdout/stderr (non-queue) or
//.job file (queue).
//Returns true if found
const bool ProcessHandler::getSshError(QString &dropMess) {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return false;
  }
  bool found = false;
  if (!mProcesschunks->isQueue()) {
    readAllStandardError();
    QTextStream textStream(mStderr);
    found = getSshError(dropMess, textStream);
  }
  else {
    if (!mProcesschunks->getCurrentDir().exists(mJobFile->fileName())) {
      return found;
    }
    if (mJobFile->size() == 0) {
      return found;
    }
    if (!mJobFile->open(QIODevice::ReadOnly)) {
      return found;
    }
    found = getSshError(dropMess, *mJobFileTextStream);
  }
  return found;
}

const bool ProcessHandler::getSshError(QString &dropMess, QTextStream &stream) {
  //look for cd error & ssh error
  QString line = stream.readLine();
  while (!line.isNull()) {
    if (line.indexOf("cd: ") != -1) {
      dropMess = "it cannot cd to %1 (%2)";
      dropMess = dropMess.arg(mProcesschunks->getRemoteDir(), line);
      return true;
    }
    else if (line.indexOf("ssh: connect to host") != -1) {
      dropMess = "cannot connect (%1)";
      dropMess = dropMess.arg(line);
      return true;
    }
    line = stream.readLine();
  }
  return false;
}

//True when the .com file has been run and it has finished.  Returns true if
//the log exists and the finished signal has been received
const bool ProcessHandler::isComProcessDone() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return false;
  }
  if (mProcesschunks->isQueue()) {
    if (!cshFileExists() && logFileExists(true)) {
      mMachine = NULL;
      if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
        mProcesschunks->getOutStream() << mDecoratedClassName << ":"
            << __func__ << ":" << mComFileJob->getComFileName()
            << ":isComProcessDone returning true (mMachine set to NULL)"
            << endl;
      }
      return true;
    }
    else {
      return false;
    }
  }
  bool done = mFinishedSignalReceived && logFileExists(true);
  if (done && mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":" << mComFileJob->getComFileName()
        << ":isComProcessDone returning " << done << endl;
  }
  return done;
}

const bool ProcessHandler::isFinishedSignalReceived() {
  return mFinishedSignalReceived;
}

//Returns true if a last line of the log file starts with "CHUNK DONE"
const bool ProcessHandler::isChunkDone() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return false;
  }
  if (!mLogFile->open(QIODevice::ReadOnly)) {
    if (logFileExists(true) && !mLogFile->open(QIODevice::ReadOnly)) {
      mProcesschunks->getOutStream() << "Warning:" << mDecoratedClassName
          << ":" << __func__ << ":Unable to open " << mLogFile->fileName()
          << endl;
    }
    return false;
  }
  int size = mLogFile->size();
  int sizeToCheck = 25;
  //Attempt to seek to the last line of the file (if the file is larger
  //then 25 characters).  If the seek fails, the whole file will have to be
  //looked at.
  if (size > sizeToCheck) {
    mLogFile->seek(size - sizeToCheck);
  }
  QByteArray lastPartOfFile = mLogFile->readAll();
  mLogFile->close();
  lastPartOfFile = lastPartOfFile.trimmed();
  bool done = lastPartOfFile.endsWith("CHUNK DONE");
  if (done && mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":" << mComFileJob->getComFileName() << ":isChunkDone returning "
        << done << ",lastPartOfFile:" << lastPartOfFile << endl;
  }
  return done;
}

void ProcessHandler::resetPausing() {
  mPausing = 0;
}

bool ProcessHandler::isPausing() {
  if (mPausing > 10) {
    return false;
  }
  mPausing++;
  return true;
}

//Reads the last 1000 characters of the file.  Returns all the text between
//with "ERROR:" and the end of the file.
void ProcessHandler::getErrorMessageFromLog(QString &errorMess) {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  if (!mLogFile->open(QIODevice::ReadOnly)) {
    return;
  }
  QTextStream stream(mLogFile);
  int size = mLogFile->size();
  if (size == 0) {
    return;
  }
  int sizeToCheck = 1000;
  if (size > sizeToCheck) {
    stream.seek(size - sizeToCheck);
  }
  QString line;
  do {
    line = stream.readLine();
    if (line.indexOf("ERROR:") != -1) {
      errorMess.append(line);
    }
  } while (!stream.atEnd());
  mLogFile->close();
  if (errorMess.isEmpty()) {
    errorMess.append("CHUNK ERROR: (last line) - ");
    errorMess.append(line);
  }
  else {
    errorMess.prepend("CHUNK ");
  }
}

//Reads the last lines of and stderr and appends then to errorMess..
void ProcessHandler::getErrorMessageFromOutput(QString &errorMess) {
  //Use the last lines of and stderr as the error message if the log
  //file is empty.
  char *eol;
#ifdef _WIN32
  eol = "\r\n";
#else
  eol = "\n";
#endif
  errorMess.append(eol);
  //stderr
  readAllStandardError();
  QByteArray output = mStderr.trimmed();
  if (!output.isEmpty()) {
    int lastLineIndex;
    lastLineIndex = output.lastIndexOf(eol);
    if (lastLineIndex != -1) {
      errorMess.append(output.mid(lastLineIndex));
      errorMess.append(eol);
    }
    else {
      errorMess.append(mStderr);
    }
  }
}

void ProcessHandler::printTooManyErrorsMessage(const int numErr) {
  mProcesschunks->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
      << __func__ << ":" << getComFileName() << " has given processing error "
      << numErr << " times - giving up" << endl;
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":mExitCode:" << mExitCode << ",mExitStatus:" << mExitStatus << endl;
  }
}

void ProcessHandler::incrementNumChunkErr() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  mComFileJob->incrementNumChunkErr();
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":incrementNumChunkErr:" << mComFileJob->getNumChunkErr() << endl;
  }
}

void ProcessHandler::printWarnings() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  if (!mLogFile->open(QIODevice::ReadOnly)) {
    return;
  }
  QByteArray line = mLogFile->readLine();
  do {
    line = mLogFile->readLine();
    if (line.indexOf("WARNING:") != -1) {
      mProcesschunks->getOutStream() << line;
    }
  } while (!mLogFile->atEnd());
  mLogFile->close();
}

const bool ProcessHandler::cshFileExists() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return false;
  }
  return mProcesschunks->getCurrentDir().exists(mComFileJob->getCshFileName());
}

const int ProcessHandler::getNumChunkErr() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return 0;
  }
  return mComFileJob->getNumChunkErr();
}

const QString ProcessHandler::getComFileName() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return "";
  }
  return mComFileJob->getComFileName();
}

const QString ProcessHandler::getLogFileName() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return "";
  }
  return mComFileJob->getLogFileName();
}

//Returns true if the process has started, but the log file hasn't been create,
//and the timeout (milliseconds) has been exceeded.
const bool ProcessHandler::isStartProcessTimedOut(const int timeout) {
  //If a process isn't running then there is nothing to timeout
  //If a process is running and the log file has been created then the log file
  //was created before the timeout
  if (!mStartingProcess) {
    return false;
  }
  if (mStartTime.elapsed() <= timeout) {
    return false;
  }
  return !logFileExists(true);
}

void ProcessHandler::setFlag(const ComFileJob::FlagType flag) {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  mComFileJob->setFlag(flag);
}

void ProcessHandler::backupLog() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  imodBackupFile(mLogFile->fileName().toLatin1().data());
}

void ProcessHandler::removeFiles() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  mProcesschunks->getCurrentDir().remove(mLogFile->fileName());
  mProcesschunks->getCurrentDir().remove(mComFileJob->getCshFileName());
  removeProcessFiles();
}

void ProcessHandler::removeProcessFiles() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  if (mProcesschunks->isQueue()) {
    mProcesschunks->getCurrentDir().remove(mJobFile->fileName());
    mProcesschunks->getCurrentDir().remove(mQidFile->fileName());
  }
  else {
    mStderr.clear();
    mPid.clear();
  }
}

QString ProcessHandler::getCshFile() {
  QString temp;
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return temp;
  }
  temp = mComFileJob->getCshFileName();
  return temp;
}

void ProcessHandler::runProcess(MachineHandler *machine) {
  //Don't run a job that has been reset.
  if (mComFileJobIndex == -1) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__) && machine
      != NULL) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":runProcess:machine:" << machine->getName() << endl;
  }
  mMachine = machine;
  int i;
  //Build command if necessary
  QString *command = NULL;
  QStringList *paramList = NULL;
  if (!mProcesschunks->isQueue()) {
    //It is not working to run processes using csh on Windows.
    if (mMachine->getName() != mProcesschunks->getHostRoot()
        && mMachine->getName() != "localhost") {
      //Create remove command
      //Original command:
      //ssh -x $sshopts $machname bash --login -c \'"cd $curdir && (csh -ef < $cshname >& $pidname ; \rm -f $cshname)"\' >&! $sshname &
      command = new QString("ssh");
      //To run remote command: bash --login -c '"command"'
      //Escape spaces in the directory path
      //Escaping the single quote shouldn't be necessary because this is not
      //being run from a shell.
      QString param = QString("\"cd %1 && (csh -ef < %2 ; \\rm -f %3)\"").arg(
          mEscapedRemoteDirPath, mComFileJob->getCshFileName(),
          mComFileJob->getCshFileName());
      paramList = new QStringList();
      paramList->append("-x");
      QStringList sshOpts = mProcesschunks->getSshOpts();
      for (i = 0; i < sshOpts.size(); i++) {
        paramList->append(sshOpts.at(i));
      }
      *paramList << mMachine->getName() << "bash" << "--login" << "-c" << param;
    }
    //hook stdin to something to avoid excessive pipes
    mProcess->setStandardOutputFile(QString("%1.stdout").arg(
        mComFileJob->getCshFileName()), QProcess::Truncate);
    mProcess->setStandardInputFile(mComFileJob->getCshFileName());
  }
  //Run command
  resetSignalValues();
  if (command != NULL) {
    if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
      mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
          << ":running remote command:" << endl << *command << " "
          << paramList->join(" ") << endl;
    }
    //Run on a remote machine
    mProcess->start(*command, *paramList);
    mProcess->closeWriteChannel();
    b3dMilliSleep(mProcesschunks->getMillisecSleep());
  }
  else {
    if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
      mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
          << ":running:" << endl << mCommand << " " << mParamList.join(" ")
          << endl;
    }
    //Run on local machine or queue
    mProcess->start(mCommand, mParamList);
    mProcess->closeWriteChannel();
    b3dMilliSleep(mProcesschunks->getMillisecSleep());
    if (mProcesschunks->isQueue()) {
      mProcess->waitForFinished(2000);
    }
  }
  //Turn on running process boolean and record start time
  mStartingProcess = true;
  mStartTime.restart();
  delete command;
  delete paramList;
}

//Kill the process.  Returns false if it started the timer and exited instead of
//called continueKillProcess.
const bool ProcessHandler::killProcess() {
  if (mMachine == NULL) {
    //Nothing to do - no process is running
    return true;
  }
  if (mComFileJob == NULL) {
    return true;
  }
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":" << mComFileJob->getComFileName() << ":A" << endl;
  }
  mKill = true;
  //Tell processchunks that a process is being killed
  //This must be done before relinquishing control.
  mProcesschunks->msgKillProcessStarted(this);
  mComFileJob->setFlag(ComFileJob::notDone);
  mRanContinueKillProcess = false;
  //Not doing the ls to handle the old RHEL5 bug - bug should be fixed
  if (mProcesschunks->isQueue() && mProcesschunks->getAns() != 'D') {
    if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
      mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
          << ":" << mComFileJob->getComFileName() << ":B" << endl;
    }
    continueKillProcess(false);
  }
  else if (!mProcesschunks->isQueue()) {
    if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
      mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
          << ":" << mComFileJob->getComFileName() << ":C" << endl;
    }
    //Make sure that the pid has been retrieved
    getPid();
    if (!mPid.isEmpty()) {
      if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
        mProcesschunks->getOutStream() << mDecoratedClassName << ":"
            << __func__ << ":" << mComFileJob->getComFileName() << ":D" << endl;
      }
      continueKillProcess(false);
    }
    else {
      if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
        mProcesschunks->getOutStream() << mDecoratedClassName << ":"
            << __func__ << ":" << mComFileJob->getComFileName() << ":E" << endl;
      }
      //if the PID isn't there yet, wait for it for at most 15 seconds
      mPidTimerId = startTimer(15 * 1000);
      return false;
    }
  }
  else {
    if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
      mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
          << ":" << mComFileJob->getComFileName() << ":F" << endl;
    }
    continueKillProcess(false);
  }
  return true;
}

void ProcessHandler::handleReadyReadStandardError() {
  if (mKill && mPidTimerId != 0) {
    //If killing, check for pid
    getPid();
    if (!mPid.isEmpty()) {
      continueKillProcess(true);
    }
  }
}

//Pid timer event
void ProcessHandler::timerEvent(const QTimerEvent */*timerEvent*/) {
  //timer should only go off once
  if (mPidTimerId != 0) {
    killTimer(mPidTimerId);
    mPidTimerId = 0;
  }
  continueKillProcess(true);
}

//If waiting for the PID (if necessary) continue killing the process
void ProcessHandler::continueKillProcess(const bool asynchronous) {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  if (!mKill) {
    mProcesschunks->getOutStream() << "Warning:" << mDecoratedClassName << ":"
        << __func__ << " called when mKill is false" << endl;
    return;
  }
  //function should only be run once
  if (mRanContinueKillProcess) {
    mProcesschunks->getOutStream() << "Warning:" << mDecoratedClassName << ":"
        << __func__ << " called when mRanContinueKillProcess is true" << endl;
    return;
  }
  mRanContinueKillProcess = true;
  if (mPidTimerId != 0) {
    killTimer(mPidTimerId);
    mPidTimerId = 0;
  }
  bool runningKillProcess = false;
  char ans = mProcesschunks->getAns();
  if (mProcesschunks->isQueue() && ans != 'D') {
    QString action(ans);
    if (ans != 'P') {
      action = "K";
    }
    //Don't know if this waits until the kill is does
    //$queuecom -w "$curdir" -a $action $comlist[$ind]:r
    //The second to last parameter is the action letter
    mParamList.replace(mParamList.size() - 2, action);
    if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
      mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
          << ":" << mCommand << " " << mParamList.join(" ") << endl;
    }
    mKillProcess->start(mCommand, mParamList);
    runningKillProcess = true;
    if (!mKillProcess->waitForFinished(1000) && mProcesschunks->isVerbose(
        mDecoratedClassName, __func__)) {
      mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
          << ":did not finish:error:" << mKillProcess->error() << ",exitCode:"
          << mKillProcess->exitCode() << ",exitStatus:"
          << mKillProcess->exitStatus() << ",state:" << mKillProcess->state()
          << endl << mKillProcess->readAllStandardError() << endl
      /* << mKillProcess->readAllStandardOutput()*/<< endl;
    }
    //Put mParamList back to its regular form
    mParamList.replace(mParamList.size() - 2, "R");
  }
  else if (!mProcesschunks->isQueue()) {
    //Make sure that the pid has been retrieved
    getPid();
    if (!mPid.isEmpty()) {
      //the pid exists - continue killing the process
      mProcesschunks->getOutStream() << "Killing "
          << mComFileJob->getComFileName() << " on " << mMachine->getName()
          << endl;
      if (mMachine->getName() == mProcesschunks->getHostRoot()
          || mMachine->getName() == "localhost") {
        //local job
        killLocalProcessAndDescendents(mPid);
      }
      else {
        //Kill a remote job in background
        //ssh -x $sshopts $machname bash --login -c \'"imodkillgroup $pid ; \rm -f $curdir/$pidname"\' &
        QString command = "ssh";
        QString param = QString("\"imodkillgroup %1\"").arg(mPid);
        QStringList paramList;
        paramList << "-x" << mProcesschunks->getSshOpts()
            << mMachine->getName() << "bash" << "--login" << "-c" << param;
        QTime time;
        mKillProcess->start(command, paramList);
        time.start();
        int sleepRet = b3dMilliSleep(mProcesschunks->getMillisecSleep());
        int timeElapsed = time.elapsed();
        mProcesschunks->getOutStream() << mDecoratedClassName << ":"
            << __func__ << ":sleepRet:" << sleepRet << ",timeElapsed:"
            << timeElapsed << endl;
        if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
          mProcesschunks->getOutStream() << mDecoratedClassName << ":"
              << __func__ << ":running " << command << " " << paramList.join(
              " ") << endl;
        }
        runningKillProcess = true;
      }
    }
  }
  if (asynchronous) {
    //MachineHandler::killNextProcess is not currently running because a wait
    //for a signal or event was done.  Run this function to get to the next
    //process.
    mMachine->killNextProcess();
  }
  if (!runningKillProcess && !mLocalKill) {
    //No kill request to wait for - go straight to clean up.
    //This happens when a queue receives a drop command, when the PID was
    //never received from a non-queue process.
    if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
      mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
          << ":Calling cleanupKillProcess from continueKillProcess" << endl;
    }
    cleanupKillProcess();
  }
}

//Sets signal variables.  For a non-queue removes the .csh file on the local
//machine.  If the process was killed, calls cleanupKillProcess.
void ProcessHandler::handleFinished(const int exitCode,
    const QProcess::ExitStatus exitStatus) {
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    printf("%s:%s:%d,exitStatus:%d\n", mDecoratedClassName.toLatin1().data(),
        __func__, exitCode, exitStatus);
  }
  if (mComFileJob == NULL) {
    printf("ERROR:%s:%s:Empty mComFileJob\n",
        mDecoratedClassName.toLatin1().data(), __func__);
    return;
  }
  mFinishedSignalReceived = true;
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    printf("%s:%s:%s\n", mDecoratedClassName.toLatin1().data(), __func__,
        mComFileJob->getComFileName().toLatin1().data());
    readAllStandardError();
  }
  mStartingProcess = false;
  mExitCode = exitCode;
  mExitStatus = exitStatus;
  //The queue request just submits the chunk to the queue, or submits a request
  //to kill the chunk to the queue.  Don't use it to figure out the state of the
  //chunk.
  if (!mProcesschunks->isQueue()) {
    if (mMachine->getName() == mProcesschunks->getHostRoot()
        || mMachine->getName() == "localhost") {
      if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
        printf("%s:%s:%s\n%s\n", mDecoratedClassName.toLatin1().data(),
            __func__,
            mProcesschunks->getCurrentDir().absolutePath().toLatin1().data(),
            mComFileJob->getCshFileName().toLatin1().data());
      }
      mProcesschunks->getCurrentDir().remove(mComFileJob->getCshFileName());
    }
    mProcesschunks->getCurrentDir().remove(QString("%1.stdout").arg(
        mComFileJob->getCshFileName()));
    if (mKill) {
      if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
        printf("%s:%s:Calling cleanupKillProcess from handleFinished\n",
            mDecoratedClassName.toLatin1().data(), __func__);
      }
      cleanupKillProcess();
    }
    mMachine = NULL;
    if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
      printf("%s:%s:machine set to NULL\n",
          mDecoratedClassName.toLatin1().data(), __func__);
    }
  }
}

void ProcessHandler::cleanupKillProcess() {
  if (mComFileJob == NULL) {
    printf("ERROR:%s:%s:Empty mComFileJob\n",
        mDecoratedClassName.toLatin1().data(), __func__);
    return;
  }
  if (!mKill) {
    printf("Warning:%s:%s called for %s when mKill is false\n",
        mDecoratedClassName.toLatin1().data(), __func__,
        mComFileJob->getComFileName().toLatin1().data());
    return;
  }
  //Tell processchunks that a process is killed
  mProcesschunks->msgKillProcessDone(this);
  mRanContinueKillProcess = false;
  if (!mKillFinishedSignalReceived) {
    mKillProcess->kill();
  }
  mKillFinishedSignalReceived = false;
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    printf(
        "Warning:%s:%s:%s:Turning off ProcessHandler::mKill in cleanupKillProcess\n",
        mDecoratedClassName.toLatin1().data(), __func__,
        mComFileJob->getComFileName().toLatin1().data());
  }
  mKill = false;
}

//Called if kill process did not finish before the Processchunks timeout
void ProcessHandler::msgKillProcessTimeout() {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  if (!mKill) {
    mProcesschunks->getOutStream() << "Warning:" << mDecoratedClassName << ":"
        << __func__ << " called when mKill is false" << endl;
    return;
  }
  //A process did not end.  Disconnect the signals and create new processes
  //to prevent slots from being run because of old processes.
  mStartingProcess = false;
  initProcess();
  resetSignalValues();
  mProcesschunks->getOutStream() << "Failed to kill "
      << mComFileJob->getComFileName() << " on " << mMachine->getName()
      << " (no problem if machine is dead)" << endl;
  if (!mKillFinishedSignalReceived) {
    mKillProcess->kill();
  }
  mKillFinishedSignalReceived = false;
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":" << mComFileJob->getComFileName()
        << ":Turning off ProcessHandler::mKill in msgKillProcessTimeout"
        << endl;
  }
  mKill = false;
}

void ProcessHandler::handleKillFinished(const int exitCode,
    const QProcess::ExitStatus exitStatus) {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":handleKillFinished: " << mComFileJob->getComFileName()
        << ",exitCode:" << exitCode << ",exitStatus:" << exitStatus << endl;
  }
  mKillFinishedSignalReceived = true;
  if (!mProcesschunks->isQueue()) {
    return;
  }
  //The queue kill request is syncronous with the job.
  bool cleanup = true;
  //exitCode == 0:  Kill is completed
  //exitCode == 100:  Process finished before it could be killed
  //exitCode == 101:  Unable to pause because the process had already started.
  if (exitCode != 0 && exitCode != 100 && exitCode != 101) {
    cleanup = false;
    mProcesschunks->getOutStream() << "kill process exitCode:" << exitCode
        << endl;
    QByteArray byteArray = mKillProcess->readAllStandardError();
    if (!byteArray.isEmpty()) {
      mProcesschunks->getOutStream() << byteArray << endl;
    }
  }
  if (cleanup) {
    if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
      mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
          << ":Calling cleanupKillProcess from handleKillFinished with exitCode:"
          << exitCode << endl;
    }
    cleanupKillProcess();
  }
}

void ProcessHandler::handleError(const QProcess::ProcessError processError) {
  if (mLocalKill) {
    //KillLocalProcessAndDescendents was used - causes a return code of 1
    return;
  }
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  mErrorSignalReceived = true;
  mProcessError = processError;
  mProcesschunks->getOutStream() << mComFileJob->getComFileName()
      << ":process error:" << processError << "," << mProcess->errorString()
      << endl;
}

void ProcessHandler::handleStarted() {
  mStartedSignalReceived = true;
}

//Stop and then kill the process with process ID pid and all of its descendents.
//Waits for kill commands to complete.
//Can't use imodkillgroup on the local host because we don't want to kill
//processchunks, and its part of the group.
void ProcessHandler::killLocalProcessAndDescendents(QString &pid) {
  if (mComFileJob == NULL) {
    mProcesschunks ->getOutStream() << "ERROR:" << mDecoratedClassName << ":"
        << __func__ << ":Empty mComFileJob" << endl;
    return;
  }
  //immediately stop the process
  stopProcess(pid);
  if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
    mProcesschunks->getOutStream() << mDecoratedClassName << ":" << __func__
        << ":stopping " << pid << endl;
  }
  int i;
  QStringList pidList;
  pidList.append(pid.trimmed());
  int pidIndex = -1;
  int ppidIndex = -1;
  QProcess ps;
  QString command("ps");
  QStringList paramList;
  paramList.append("axl");
  //Run ps and stop descendent processes until there is nothing left to stop.
  bool foundNewChildPid = false;
  do {
    //Run ps axl
    ps.start(command, paramList);
    ps.waitForFinished(5000);
    if (!ps.exitCode()) {
      QTextStream stream(ps.readAllStandardOutput().trimmed());
      if (!stream.atEnd()) {
        //Find the pid and ppid columns if they haven't already been found.
        QString header;
        if (pidIndex == -1 || ppidIndex == -1) {
          header = stream.readLine().trimmed();
          if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
            mProcesschunks->getOutStream() << mDecoratedClassName << ":"
                << __func__ << ":ps column header:" << endl << header << endl;
          }
          QStringList headerList = header.split(QRegExp("\\s+"),
              QString::SkipEmptyParts);
          for (i = 0; i < headerList.size(); i++) {
            if (headerList.at(i) == "PID") {
              pidIndex = i;
            }
            else if (headerList.at(i) == "PPID") {
              ppidIndex = i;
            }
            if (pidIndex != -1 && ppidIndex != -1) {
              break;
            }
          }
        }
        //collect child pids
        if (pidIndex != -1 && ppidIndex != -1) {
          foundNewChildPid = false;
          do {
            QString line = stream.readLine().trimmed();
            QStringList columns = line.split(QRegExp("\\s+"),
                QString::SkipEmptyParts);
            if (columns.size() < pidIndex || columns.size() < ppidIndex) {
              break;
            }
            //Look for child processes of parent processes that have already been
            //found.  Stop them and add them to the process ID list.  If
            //the process ids are in order, all the descendent pids should be
            //found during the first pass.
            if (pidList.contains(columns.at(ppidIndex))) {
              QString childPid = columns.at(pidIndex);
              if (!pidList.contains(childPid)) {
                stopProcess(childPid);
                foundNewChildPid = true;
                pidList.append(childPid);
                if (mProcesschunks->isVerbose(mDecoratedClassName, __func__)) {
                  mProcesschunks->getOutStream() << mDecoratedClassName << ":"
                      << __func__ << ":stopping:" << endl << line << endl;
                }
              }
            }
          } while (!stream.atEnd());
        }
        else {
          mProcesschunks->getOutStream()
              << "Warning: May not have been able to kill all processes descendent from "
              << mCommand << " " << mComFileJob->getCshFileName()
              << " on local machine.  Ps PID and PPID columns where not found in "
              << header << "(" << command << " " << paramList.join(" ") << ")."
              << endl;
          return;
        }
      }
      else {
        mProcesschunks->getOutStream()
            << "Warning: May not have been able to kill all processes descendent from "
            << mCommand << " " << mComFileJob->getCshFileName()
            << " on local machine.  Ps command return nothing" << "("
            << command << " " << paramList.join(" ") << ")." << endl;
      }
    }
    else {
      mProcesschunks->getOutStream()
          << "Warning: May not have been able to kill all processes descendent from "
          << mCommand << " " << mComFileJob->getCshFileName()
          << " on local machine.  Ps command failed" << "(" << command << " "
          << paramList.join(" ") << ")." << endl;
      return;
    }
  } while (foundNewChildPid);
  mLocalKill = true;
  //Kill everything in the process ID list.
  for (i = 0; i < pidList.size(); i++) {
    killProcess(pidList.at(i));
  }
}

//Stop a single process.  Waits for process to complete.
void ProcessHandler::stopProcess(const QString &pid) {
  QProcess ps;
  QString command("kill");
  QStringList paramList;
  paramList.append("-19");
  paramList.append(pid);
  ps.execute(command, paramList);
}

//Kill a single process.  Waits for process to complete.
void ProcessHandler::killProcess(const QString &pid) {
  QProcess ps;
  QString command("kill");
  QStringList paramList;
  paramList.append("-9");
  paramList.append(pid);
  ps.execute(command, paramList);
}

