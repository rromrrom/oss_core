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


#ifndef SIP_SIPStack_INCLUDED
#define SIP_SIPStack_INCLUDED


#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <list>

#include "OSS/OSS.h"

#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/Net.h"
#include "OSS/Net/DNS.h"
#include "OSS/UTL/Thread.h"
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/SIPTransportService.h"


namespace OSS {
namespace SIP {


class OSS_API SIPStack
  /// This class is the main entry point for all the SIP Stack functions.
  ///
  /// There can be multiple instances of SIPStack objects in a single application
  /// making it possible to create a multitude of different UA types all residing
  /// inside a homegenous application namespace
{
public:
  
  typedef std::map< std::string, SIPListener::SubNets > SubNets;

  SIPStack();
    /// Creates a new SIPStack Object

  ~SIPStack();
    /// Destroys the SIPStack Object;
 
  bool& enableUDP();
    /// Set this to true/false to enable or disable UDP transport.
    ///
    /// This must be set before calling the SIPStack::run() method.
  
  bool& enableTCP();
    /// Set this to true/false to enable or disable TLS transport.
    ///
    /// This must be set before calling the SIPStack::run() method.

  bool& enableTLS();
    /// Set this to true/false to enable or disable TLS transport.
    ///
    /// This must be set before calling the SIPStack::run() method.

  OSS::socket_address_list& udpListeners();
    /// Returns the UDP Listener vector.
    ///
    /// All interfaces where the UDP listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts 
    /// a tuple object of type SIPStack::OSS::Net::IPAddress.
    ///
    /// This must be set before calling the SIPStack::run() method.

  OSS::socket_address_list& tcpListeners();
    /// Returns the TCP Listener vector.
    ///
    /// All interfaces where the TCP listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts 
    /// a tuple object of type OSS::OSS::Net::IPAddress.
    ///
    /// This must be set before calling the SIPStack::run() method

  OSS::socket_address_list& wsListeners();
    /// Returns the WebSocket Listener vector.
    ///
    /// All interfaces where the WebSocket listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts
    /// a tuple object of type OSS::OSS::Net::IPAddress.
    ///
    /// This must be set before calling the SIPStack::run() method

  OSS::socket_address_list& tlsListeners();
    /// Returns the TLS Listener vector.
    ///
    /// All interfaces where the TLS listener should bind to
    /// must be push_back()ed into this vector.  The vector accepts 
    /// a tuple object of type OSS::OSS::Net::IPAddress.
    ///
    /// This must be set before calling the SIPStack::run() method

  void transportInit();
    /// Initialize the SIP Transport.
    ///
    /// This function must be called before a call to run()
    /// If initTransportFromConfig() is used, transportInit will be called
    /// automatically so there is no need to call it before calling run.


  void transportInit(unsigned short udpPortBase, unsigned short udpPortMax,
    unsigned short tcpPortBase, unsigned short tcpPortMax,
    unsigned short wsPortBase, unsigned short wsPortMax,
    unsigned short tlsPortBase, unsigned short tlsPortMax);
    /// Initialize the SIP Transport.  This is similar to transportInit()
    /// Except that ports are determined based on the first available port
    /// within the given port ranges.   The transport vectors are
    /// updated accordingly as a successful binding is obtained.
    ///
    /// This function must be called before a call to run()
    /// If initTransportFromConfig() is used, transportInit will be called
    /// automatically so there is no need to call it before calling run.

#if ENABLE_FEATURE_CONFIG
  void initTransportFromConfig(const boost::filesystem::path& cfgFile);
    /// Initialize the sip stack properties from a preexisting CFG files.
    /// This method will throw PersistenceException if the file is none
    /// existent or the file can't be parsed
#endif

#if 0
  void initTransportFromConfig(const boost::filesystem::path& cfgFile,
    unsigned short udpPortBase, unsigned short udpPortMax,
    unsigned short tcpPortBase, unsigned short tcpPortMax,
    unsigned short tlsPortBase, unsigned short tlsPortMax);
    /// Initialize the sip stack properties from a preexisting CFG files.
    /// This method will throw PersistenceException if the file is none
    /// existent or the file can't be parsed. This is similar to initTransportFromConfig()
    /// Except that ports are determined based on the first available port
    /// within the given port ranges.   The transport vectors are
    /// updated accordingly as a successful binding is obtained.
#endif
  
  bool initializeTlsContext(
    const std::string& tlsCertFile, // Certificate to be used by this server.  File should be in PEM format
    const std::string& privateKey, // Private key to be used by this server.  File should be in PEM format
    const std::string& tlsCertFilePassword, // Set this value if tlsCertFile is password protected
    const std::string& peerCaFile, // If the remote peer this server is connecting to uses a self signed certificate, this file is used to verify authenticity of the peer identity
    const std::string& peerCaPath, // A directory full of CA certificates. The files must be named with the CA subject name hash value. (see man SSL_CTX_load_verify_locations for more info)
    bool verifyPeer // Verify the peer certificates.  If the peer CA file is not set, set this value to false
  );
    /// This method intializes the TLS context if secure transport is enabled
  
 #if ENABLE_FEATURE_CONFIG 
  bool initTlsContextFromConfig(const boost::filesystem::path& cfgFile);
    /// Initialize TLS using a configuration file
#endif
  
  std::string getTlsCertPassword() const;
    /// Returns the tlsCertPassword.  This is used internally by initializeTlsContext
#if ENABLE_FEATURE_CONFIG
  bool initVirtualTransportFromConfig(const boost::filesystem::path& cfgFile);
    /// Initialize CARP virtual interface(s)
#endif
  
  void run();
    /// Starts the SIPStack event subsytem.
    ///
    /// This will block until a call to stop() received.
    /// Once the run method has exited, it is 
    /// possible to run() the system again after changing
    /// the properties of SIPStack.  One good example
    /// is when a there is a need to change listener address.
    ///
    /// This will throw an exception if a problem is encountered.

  void stop();
    /// Stops the SIPStack event subsytem.
    ///
    /// This function will block until both the fsm and transport
    /// subsystems have abandoned all pending work safely.
    ///
    /// All pending call to read and write methods will return with 
    /// an io exception.

  void sendRequest(
    const SIPMessage::Ptr& pRequest,
    const OSS::Net::IPAddress& localAddress,
    const OSS::Net::IPAddress& remoteAddress,
    SIPTransaction::Callback callback,
    SIPTransaction::TerminateCallback terminateCallback);
    /// Send a new SIP (REQUEST) message to the Network.
    ///
    /// This is a none-blocking function call for sending
    /// SIP (REQUEST) messages to the network.  The local interface
    /// to be used must be always specified to add extra
    /// flexibility for applications that bridges multi-homed
    /// networks with complex routing rules.
    ///
    /// The remote address must be in the form of an IP address.
    /// SIPStack supports both IPV4 and IPV6 destinations.
    /// DNS lookup will not be performed by the transport layer.
    /// Thus, this method expects that the remote address has 
    /// already been resolved using the mechanisms exposed by
    /// OSSADNS or a third party DNS client.
    ///
    /// Since the actual tranport address to be used for the request via
    /// is yet to be known from the transport layer at this point (specially TCP and TLS),
    /// the via address will be modified automatically by the transport.
    /// Existing parameters in the via will not be changed, neither would
    /// the transport add any to the existing via parameters.  Parameters
    /// such as branch and rport MUST already be present in the existing via.
    ///
    /// This function call is a none blocking call.  All responses
    /// will be sent back through the SIPTransaction::Callback function.
    /// If an error occured, the callback function will receive
    /// the SIPException as the first parameter and must always be checked
    /// prior to processing of the rest of the callback parameters.
    /// Normal cause of errors are transaction timeouts.
    ///
    /// The _terminateCallback functor can be provided if the upper
    /// layer wants to be notified when the client transaction terminates
    ///
    /// This function may throw a SIPException if the request cannot be processed.
    ///


  void sendRequestDirect(const SIPMessage::Ptr& pRequest,
    const OSS::Net::IPAddress& localAddress,
    const OSS::Net::IPAddress& remoteAddress);
    /// This method will send the SIPMessage to the target
    /// without creating a transaction.  No transaction means
    /// the message will not be retransmitted as well as
    /// responses to the message (if it is a request) will
    /// not be tracked by a callback method.  This function
    /// is normally used to relay orphaned requests like ACK
    /// and 200 OK for INVITE transactions.


  void setRequestHandler(const SIPTransaction::RequestCallback& handler);
    /// This function sets the callback handler for incoming requests.
    ///
    /// When a new server is created, the transaction will propagate
    /// the request to the application layer via this callback.
    ///
    /// If this callback is not set, the request will be silently dropped.

  void setAckOr2xxTransactionHandler(const SIPFSMDispatch::UnknownTransactionCallback& handler);
    /// This function sets the callback handler for ACK and 200 OK retranmissions.
    ///
    /// If this callback is not set, the request will be silently dropped.


  SIPTransportService& transport();
    /// Return a reference to the transport service
  
  void setTransportThreshold(
    unsigned long packetsPerSecondThreshold, // The total packets per second threshold
    unsigned long thresholdViolationRate, // Per IP threshold
    int banLifeTime // violator jail lifetime
  );
  
  SIPTransaction::Ptr createClientTransaction(const SIPMessage::Ptr& pRequest);
    /// Create a new transaction for a new non-ACK outgoing request
  
private:

  //
  // FSM Parameters
  //

  SIPFSMDispatch _fsmDispatch;

  //
  // Transport Parameters
  //
  bool _enableUDP;
  bool _enableTCP;
#if ENABLE_FEATURE_WEBSOCKETS
  bool _enableWS;
#endif
  bool _enableTLS;
  
  OSS::socket_address_list _udpListeners;
  OSS::socket_address_list _tcpListeners;
  OSS::socket_address_list _wsListeners;
  OSS::socket_address_list _tlsListeners;
  
  SubNets _udpSubnets;
  SubNets _tcpSubnets;
  SubNets _wsSubnets;
  SubNets _tlsSubnets;
  
  std::string _tlsCertPassword;
};

typedef SIPStack SIPStack;

//
// Inlines
//

inline bool& SIPStack::enableUDP()
{
  return _enableUDP;
}
  
inline bool& SIPStack::enableTCP()
{
  return _enableTCP;
}

inline bool& SIPStack::enableTLS()
{
  return _enableTLS;
}

inline OSS::socket_address_list& SIPStack::udpListeners()
{
  return _udpListeners;
}

inline OSS::socket_address_list& SIPStack::tcpListeners()
{
  return _tcpListeners;
}

inline OSS::socket_address_list& SIPStack::wsListeners()
{
  return _wsListeners;
}

inline OSS::socket_address_list& SIPStack::tlsListeners()
{
  return _tlsListeners;
}


inline void SIPStack::setRequestHandler(const SIPTransaction::RequestCallback& handler)
{
  _fsmDispatch.requestHandler() = handler;
}

inline void SIPStack::setAckOr2xxTransactionHandler(const SIPFSMDispatch::UnknownTransactionCallback& handler)
{
  _fsmDispatch.ackOr2xxTransactionHandler() = handler;
}

inline SIPTransportService& SIPStack::transport()
{
  return _fsmDispatch.transport();
}

inline std::string SIPStack::getTlsCertPassword() const
{
  return _tlsCertPassword;
}

inline SIPTransaction::Ptr SIPStack::createClientTransaction(const SIPMessage::Ptr& pRequest)
{
  return _fsmDispatch.createClientTransaction(pRequest);
}


} } // OSS::SIP
#endif // SIP_SIPStack_INCLUDED
