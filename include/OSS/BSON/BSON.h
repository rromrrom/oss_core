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

#ifndef OSS_BSON_H_INCLUDED
#define OSS_BSON_H_INCLUDED


#include "OSS/OSS.h"
#include "OSS/BSON/BSONValue.h"
#include "OSS/BSON/BSONString.h"
#include "OSS/BSON/BSONBool.h"
#include "OSS/BSON/BSONDouble.h"
#include "OSS/BSON/BSONInt32.h"
#include "OSS/BSON/BSONInt64.h"
#include "OSS/BSON/BSONArray.h"
#include "OSS/BSON/BSONDocument.h"
#include "OSS/BSON/BSONIterator.h"
#include "OSS/BSON/BSONParser.h"

#include "OSS/build.h"
#if ENABLE_FEATURE_ZMQ
#include "OSS/BSON/BSONQueue.h"
#endif


#endif //OSS_BSON_H_INCLUDED

