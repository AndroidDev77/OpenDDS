/*
 * $Id$
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */
#include "DCPS/DdsDcps_pch.h" //Only the _pch include should start with DCPS/
#include "dcps_export.h"

#include "Recorder.h"

namespace OpenDDS {
namespace DCPS {

RecorderListener::~RecorderListener()
{
}

Recorder::~Recorder()
{
}

Recorder_ptr Recorder::_duplicate(Recorder_ptr obj)
{
  if (obj) obj->_add_ref();
  return obj;
}

}
}

OpenDDS::DCPS::Recorder_ptr
TAO::Objref_Traits<OpenDDS::DCPS::Recorder>::duplicate(OpenDDS::DCPS::Recorder_ptr p)
{
  return OpenDDS::DCPS::Recorder::_duplicate(p);
}

void
TAO::Objref_Traits<OpenDDS::DCPS::Recorder>::release(OpenDDS::DCPS::Recorder_ptr p)
{
  if (p) p->_remove_ref();
}

OpenDDS::DCPS::Recorder_ptr
TAO::Objref_Traits<OpenDDS::DCPS::Recorder>::nil()
{
  return static_cast<OpenDDS::DCPS::Recorder_ptr>(0);
}

CORBA::Boolean
TAO::Objref_Traits<OpenDDS::DCPS::Recorder>::marshal(const OpenDDS::DCPS::Recorder_ptr,
                                                     TAO_OutputCDR&)
{
  return false;
}
