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

#ifndef OSS_RAFTNODE_H_INCLUDED
#define	OSS_RAFTNODE_H_INCLUDED


#include "OSS/OSS.h"
#include <boost/noncopyable.hpp>

extern "C" 
{ 
  #include "OSS/RAFT/libraft.h" 
};


namespace OSS {
namespace RAFT {

  
class RaftNode
{
public:
  RaftNode();
  RaftNode(const RaftNode& node);
  RaftNode(raft_node_t* node);
  ~RaftNode();

  RaftNode& operator=(const RaftNode& node);
  RaftNode& operator=(raft_node_t* node);

  int getId() const;
  int getNextIndex() const;
  int getMatchIndex() const;
  void* getUserData() const;
  void setUserData(void* userData);
  raft_node_t* node();

private:
  raft_node_t* _node;
};

//
// Inlines
//

inline raft_node_t* RaftNode::node()
{
  return _node;
}


} } // OSS::RAFT

#endif	// OSS_RAFTNODE_H_INCLUDED

