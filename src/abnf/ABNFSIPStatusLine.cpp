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


#include "OSS/ABNF/ABNFSIPStatusLine.h"
#include "OSS/ABNF/ABNFSIPVersion.h"
#include "OSS/ABNF/ABNFSIPStatusCode.h"
#include "OSS/ABNF/ABNFSIPReasonPhrase.h"


namespace OSS {

namespace ABNF {


/// Satisfies RFC 3261 ABNF Rule for 
/// Status-Line =  SIP-Version SP Status-Code SP Reason-Phrase CRLF
typedef ABNFLRSequence6<
  ABNF_SIP_Version,
  ABNF_SIP_SP,
  ABNF_SIP_Status_Code,
  ABNF_SIP_SP,
  ABNF_SIP_Reason_Phrase,
  ABNFLROptional<ABNF_SIP_CRLF> >Parser;
static Parser _parser;

ABNFSIPStatusLine::ABNFSIPStatusLine()
{
}

char* ABNFSIPStatusLine::parse(const char* _t)
{
  return _parser.parse(_t);
}

char* ABNFSIPStatusLine::parseTokens(const char* _t, std::vector<std::string>& tokens)
{
  return _parser.parseTokens(_t, tokens);
}



} } //OSS::ABNF



