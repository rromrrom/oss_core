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

#ifndef OSS_BSONDOCUMENT_H_INCLUDED
#define OSS_BSONDOCUMENT_H_INCLUDED


#include "OSS/BSON/BSONValue.h"


namespace OSS {
namespace BSON {

  
class BSONDocument : public BSONValue
{
public:
  BSONDocument();
  BSONDocument(const BSONDocument& value);
  explicit BSONDocument(const Document& value);
  void swap(BSONDocument& value);
  BSONDocument& operator=(const Document& value);
  BSONDocument& operator=(const BSONDocument& value);
  Document& value();
  const Document& value() const;
};

//
// Inlines
//
inline void BSONDocument::swap(BSONDocument& value)
{
  std::swap(_value, value._value);
}

inline BSONValue::Document& BSONDocument::value()
{
  return asDocument();
}

inline const BSONValue::Document& BSONDocument::value() const
{
  return asDocument();
}

} } // OSS::BSON


#endif // OSS_BSONDOCUMENT_H_INCLUDED

