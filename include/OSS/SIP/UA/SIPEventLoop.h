// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "GNU Lesser General Public License (LGPL)".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef OSS_SIPEVENTLOOP_H_INCLUDED
#define	OSS_SIPEVENTLOOP_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_LIBRE

#include "OSS/UTL/CoreUtils.h"

namespace OSS {
namespace SIP {
namespace UA {


class SIPEventLoop : boost::noncopyable
{
public:
  SIPEventLoop();
  ~SIPEventLoop();
  
  void run(bool async);
  void stop(bool closeDescriptors = false, bool memCheck = false, bool tmrCheck = false);
  void waitForTermination();
  
private:
  void internal_run();
  boost::thread* _pRunThread;
  bool _memCheck;
  bool _tmrCheck;
  bool _closeDescriptors;
};

} } } // OSS::SIP::UA

#endif // ENABLE_FEATURE_LIBRE

#endif	// OSS_SIPEVENTLOOP_H_INCLUDED

