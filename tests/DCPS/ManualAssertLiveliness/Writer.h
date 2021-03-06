// -*- C++ -*-
//
// $Id$
#ifndef WRITER_H
#define WRITER_H

#include <dds/DdsDcpsPublicationC.h>
#include <ace/Task.h>
#include "dds/DCPS/PoolAllocator.h"

class Writer_Base : public ACE_Task_Base
{
public:
  Writer_Base (::DDS::DataWriter_ptr writer);
  void start ();
  void end ();
protected:
  ::DDS::DataWriter_var writer_;
  OPENDDS_STRING id_;
  const char* get_id() const { return id_.c_str(); }
};


// via write()
class Manual_By_Participant_Writer_1 : public Writer_Base
{
public:

  Manual_By_Participant_Writer_1 (::DDS::DataWriter_ptr writer);

  /** Lanch a thread to write. **/
  virtual int svc ();
};


// via participant->assert_liveliness()
class Manual_By_Participant_Writer_2 : public Writer_Base
{
public:

  Manual_By_Participant_Writer_2 (::DDS::DataWriter_ptr writer);

  /** Lanch a thread to write. **/
  virtual int svc ();

private:

  ::DDS::DomainParticipant_var participant_;
};



// via write()
class Manual_By_Topic_Writer_1 : public Writer_Base
{
public:

  Manual_By_Topic_Writer_1 (::DDS::DataWriter_ptr writer);

  /** Lanch a thread to write. **/
  virtual int svc ();
};


// via assert_liveliness ()
class Manual_By_Topic_Writer_2 : public Writer_Base
{
public:

  Manual_By_Topic_Writer_2 (::DDS::DataWriter_ptr writer);

  /** Lanch a thread to write. **/
  virtual int svc ();
};


#endif /* WRITER_H */
