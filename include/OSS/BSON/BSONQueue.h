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

#ifndef OSS_BSONQUEUE_H_INCLUDED
#define	OSS_BSONQUEUE_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_ZMQ

#include "OSS/BSON/BSONParser.h"
#include "OSS/BSON/BSONDocument.h"
#include "OSS/ZMQ/ZMQSocket.h"

namespace OSS {
namespace BSON {


class BSONQueue : boost::noncopyable
{
public:
  enum Role
  {
    PRODUCER,
    CONSUMER
  };
  
  BSONQueue(Role role, const std::string& name);
  ~BSONQueue();
  bool enqueue(BSONParser& msg);
  bool dequeue(BSONParser& msg);
  bool enqueue(BSONDocument& msg);
  bool dequeue(BSONDocument& msg);
  const std::string& getName() const;
    
protected:
  virtual bool initSocket();
  Role _role;
  std::string _name;
  std::string _address;
  ZMQ::ZMQSocket* _pSocket;
};


//
// Inlines
//
    
} } // OSS::BSON

#endif // ENABLE_FEATURE_ZMQ
#endif	// OSS_BSONQUEUE_H_INCLUDED

