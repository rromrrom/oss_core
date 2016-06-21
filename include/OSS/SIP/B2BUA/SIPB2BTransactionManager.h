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

#ifndef SIP_SIPStackB2BTransactionManager_INCLUDED
#define SIP_SIPStackB2BTransactionManager_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_B2BUA

#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include "OSS/OSS.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/Net/Net.h"
#include "OSS/UTL/Thread.h"
#include "OSS/SIP/B2BUA/B2BUA.h"
#include "OSS/SIP/SIP.h"
#include "OSS/SIP/SIPStack.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/EP/SIPEndpoint.h"
#include "OSS/SIP/B2BUA/SIPB2BTransaction.h"
#include "OSS/SIP/B2BUA/SIPB2BHandler.h"
#include "OSS/SIP/B2BUA/SIPB2BUserAgentHandlerList.h"
#include "OSS/Persistent/RESTKeyValueStore.h"
#include "OSS/SIP/B2BUA/SIPB2BRegisterAgent.h"


namespace OSS {
namespace SIP {
namespace B2BUA {


class OSS_API SIPB2BTransactionManager : public OSS::SIP::EP::SIPEndpoint
{
public:
  typedef std::map<SIPB2BHandler::MessageType, SIPB2BHandler::Ptr> MessageHandlers;
  typedef std::map<std::string, SIPB2BHandler::Ptr> DomainRouters;
  typedef boost::function<void(SIPB2BTransactionManager*, SIPB2BTransaction*)> ExternalDispatch;
  
  typedef boost::function<SIPMessage::Ptr(
    SIPMessage::Ptr& pRequest,
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target)> PostRouteCallback;

  SIPB2BTransactionManager(int minThreadCount = 2, int maxThreadCount = 1024);
    /// Creates a new SIPB2BTransactionManager object

  virtual ~SIPB2BTransactionManager();
    /// Destroys the SIPB2BTransactionManager Object

  void initialize(const boost::filesystem::path& cfgDirectory);
    /// Initialize the manager configuration using the configuration path specified.
    /// If an error occurs, this method will throw a PersistenceException.
    ///
    /// Take note that the configuration directory must be both readable and writeble
    /// by the user that owns the process.

  void deinitialize();
    /// Deinitialize the manager.  This is usually called when the application
    /// is about the exit.  This is the place where the manager performs final
    /// trash management.

  SIPMessage::Ptr onTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);
    /// Called by runtask signalling the creation of the transaction.
    /// This prcedes any other transaction callbacks and therefore is the best place
    /// to initialize anything that would be needed by the transaction processing

  SIPMessage::Ptr onAuthenticateTransaction(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);
    /// Authenticate the new Transaction request,
    ///
    /// This method is called from the B2B Transaction runTask().
    /// The intention of the virtual function is to create a
    /// proxy-auth response.  If a null Ptr is returned by this
    /// callback, runTask will proceed in processing the request
    /// without authenticating it.
    ///
    ///
    /// Take note that authentication state is not maintained
    /// by the transaction.  It is the responsibility of the application
    /// to maintain the authentication state.

  virtual bool onRouteResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest, 
    const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& target);
    /// This method allows the application layer to determine
    /// the target address of the response.  The default behavior
    /// would use the source address of the request if the via
    /// sentby is a private host.  Via with public sentby will be honored.
    /// If useSourceAddressForResponses is set to true, the response will be routed
    /// back to the source address of the request regrdless whether its a private or public
    /// address.

  virtual SIPMessage::Ptr onGenerateLocalResponse(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    SIPB2BTransaction::Ptr pTransaction);
    /// This will be called by the B2BUA if the transaction property "generate-local-response"
    /// is set to 1 by the route handler.  This is used by the applicaiton layer to allow
    /// transactions to directly respond to the request without waiting for the remote
    /// UA to send its own response

  virtual SIPMessage::Ptr onInvokeLocalHandler(
    const OSS::SIP::SIPMessage::Ptr& pRequest,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport,
    SIPB2BTransaction::Ptr pTransaction);
    /// This will be called by the B2BUA if the transaction property "invoke-local-handler"
    /// is set to 1 by the route handler.  This is used by the applicaiton layer to allow
    /// transactions to process a request locally.  Example is local registration.


  SIPMessage::Ptr onRouteTransaction(
    SIPMessage::Ptr& pRequest, 
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
    /// Route the new request.
    ///
    /// This method expects that the application will format the request-uri
    /// towards the intended target, insert the correct via and contact where responses,
    /// will be received as well as insert a route-set if
    /// upstream proxies are needed.
    ///
    /// If the return value is an error response, the transaction
    /// will send it automatically to the sender.  Eg 404, if no route exists.
    /// If the request is routable, the return value must be a null-Ptr.
    ///
    /// Both Local Interface and Target address that the request would use
    /// must be set by the application layer

  SIPMessage::Ptr onProcessRequestBody(
    SIPMessage::Ptr& pRequest, 
    SIPB2BTransaction::Ptr pTransaction);
    /// This method allows the application to 
    /// process the body of the request
    /// before it gets sent out.
    ///
    /// This method allows the upper layer to modify the body
    /// based on specific application requirements.  For
    /// example, a media proxy may modify SDP address and port
    /// so that RTP passes through the application.
    ///
    /// If the return value is an error response, the transaction
    /// will send it automatically to the sender.
    ///
    /// If the body is supported, the return value must be a null-Ptr.

  void onProcessResponseBody(
    SIPMessage::Ptr& pRequest, 
    SIPB2BTransaction::Ptr pTransaction);
    /// This method allows the application to 
    /// process the body of the response
    /// before it gets sent out.
    ///
    /// This method allows the upper layer to modify the body
    /// based on specific application requirements.  For
    /// example, a media proxy may modify SDP address and port
    /// so that RTP passes through the application.
    ///


  void onProcessOutbound(
    SIPMessage::Ptr& pRequest, 
    SIPB2BTransaction::Ptr pTransaction);
    /// This is the last chance for the application to process
    /// the outbound request before it gets sent out to the transport.
    ///
    /// This is normally the place where application would want to
    /// insert application-specific headers as well as change existing
    /// headers to the desired application-specific values for as long
    /// as it wont conflict with dialog creation states.

  void onProcessResponseInbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);
    /// Process the newly received response

  void onProcessResponseOutbound(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);
    /// This is the last chance for the application to process
    /// the outbound response before it gets sent out to the transport.
    ///
    /// This is normally the place where application would want to
    /// insert application-specific headers as well as change existing
    /// headers to the desired application-specific values for as long
    /// as it wont conflict with dialog creation states.

  void onTransactionError(
    OSS::SIP::SIPTransaction::Error e,
    SIPMessage::Ptr pErrorResponse, 
    SIPB2BTransaction::Ptr pTransaction);
    /// Signals that an error occured on the transaction
    /// 
    /// The transaction will be detroyed automatically after this function call

  void onDestroyTransaction(SIPB2BTransaction::Ptr pTransaction);
    /// Signals that trhe transaction is about to be destroyed.
    /// This function will not invalidate the shared pointers
    /// to the transaction.  It is a mere indication that the transaction
    /// thread would now destroy its internal reference to the transaction.

  void registerHandler(SIPB2BHandler::Ptr handler);
    /// Register a message handler.
    /// Take note that there could only be one message handler per
    /// type of message.  If the handler already exists, it will
    /// be ovewritten.  Also take note that message handlers
    /// must be registered only during the initialization phase
    /// since it is not guarged by any mutex, adding or removing
    /// handlers when transactions are already active will
    /// most likely result to a corrupt map container and invalidate
    /// iterators
  
  void registerDomainRouter(const std::string& domain, SIPB2BHandler::Ptr handler);
    /// Register a specific handler for routing messages for a particular domain

  void registerDefaultHandler(SIPB2BHandler* pDefaultHandler);
    /// Register a default handler. This special handler will take care of all
    /// events that are not handled by specific message handlers
    ///

  OSS::thread_pool& threadPool();
    /// Returns a direct reference to the thread pool

  bool& useSourceAddressForResponses();

  MessageHandlers& handlers();
    /// return a reference to the handlers

  void setSipConfigFile(const std::string& sipConfigFile);
    /// Set the file name of the sip configuration file.
    /// This defaults to sip.cfg
  
  void sendClientRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg);
    /// Sends a locally initiated client request

  bool onClientTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);
    /// Called by runtask signalling the creation of the transaction.
    /// This prcedes any other transaction callbacks and therefore is the best place
    /// to initialize anything that would be needed by the transaction processing

  bool onRouteClientTransaction(
    SIPMessage::Ptr& pRequest,
    SIPB2BTransaction::Ptr pTransaction,
    OSS::Net::IPAddress& localInterface,
    OSS::Net::IPAddress& target);
    /// Route the new request.
    ///
    /// This method expects that the application will format the request-uri
    /// towards the intended target, insert the correct via and contact where responses,
    /// will be received as well as insert a route-set if
    /// upstream proxies are needed.
    ///
    /// If the return value is an error response, the transaction
    /// will send it automatically to the sender.  Eg 404, if no route exists.
    /// If the request is routable, the return value must be a null-Ptr.
    ///
    /// Both Local Interface and Target address that the request would use
    /// must be set by the application layer
  void onClientTransactionError(
    OSS::SIP::SIPTransaction::Error e,
    SIPMessage::Ptr pErrorResponse,
    SIPB2BTransaction::Ptr pTransaction);
    /// Signals that an error occured on the transaction
    ///
    /// The transaction will be detroyed automatically after this function call
  void onProcessClientResponse(
    SIPMessage::Ptr& pResponse,
    SIPB2BTransaction::Ptr pTransaction);
    /// Process the newly received response

  void setPostRouteCallback(const PostRouteCallback& postRouteCallback);
    /// This sets a callback to allow applications to still process a request
    /// after it has been routed by the designated handler

  const PostRouteCallback& getPostRouteCallback() const;
    /// Returns a constat reference to the post route callback

  void addUserAgentHandler(SIPB2BUserAgentHandler* pHandler);
    /// Register a user agent handler

  bool registerPlugin(const std::string& name, const std::string& path);
    /// Register a plugin.
    ///
    /// This function registers a UserAgent handler derrived from
    /// SIPB2BUserAgent handler from a .so file.  Handlers can hijack back to back
    /// transactions before they get processed by the scripting layer.
    /// If a handler "handled" the transaction, the scripting layer
    /// will no longer process the transaction and instead, let the handler
    /// respond to the transaction.
  
#if ENABLE_FEATURE_LIBRE
  bool startLocalRegistrationAgent(
    const std::string& agentName,
    const std::string& route,
    const OSS::SIP::UA::SIPUserAgent::ExitHandler& exitHandler);
    /// Starts the local registration user agent.  This method
    /// must be called prior to calling sendLocalRegister.
    /// The exitHandler is a callback function that is notified
    /// when all transactions has ended after calling stopLocalRegistrationAgent
  
  void stopLocalRegistrationAgent();
    /// Stops the local registration agent.  Notifies the exit handler
    /// after all transactions have ended
  
  bool sendLocalRegister(
    const std::string& user,
    const std::string& authUser,
    const std::string& authPassword,
    const std::string& domain,
    OSS::UInt32 expires,
    const std::string& registrarAddress
  );
    /// Register with a remote domain
  
  bool isForLocalRegistration(const std::string& contact);
    /// Returns true if the SIPMessage request-uri is for a locally
    /// registered account
  
  void onLocalRegisterResponse(
    OSS::SIP::UA::SIPRegistration* pReg, 
    const SIPMessage::Ptr& pMsg, 
    const std::string& error);
    /// Notified when a response is received for a local register

#endif
   
protected:
  void handleRequest(
    const OSS::SIP::SIPMessage::Ptr& pMsg, 
    const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);
    /// This is the incoming request callback that will be attached to the stack

  void handleAckOr2xxTransaction(
    const OSS::SIP::SIPMessage::Ptr& pMsg,
    const OSS::SIP::SIPTransportSession::Ptr& pTransport);
    /// Handler of ACK and 200 Ok retransmission

  virtual SIPB2BTransaction* onCreateB2BTransaction(
    const OSS::SIP::SIPMessage::Ptr& pMsg, 
    const OSS::SIP::SIPTransportSession::Ptr& pTransport, 
    const OSS::SIP::SIPTransaction::Ptr& pTransaction);
    /// Return a newly created SIPB2BTransaction object.
    /// Implementors may choose to override this and return their
    /// own custom SIPB2BTransaction subclass.

  virtual SIPB2BTransaction* onCreateB2BClientTransaction(
    const OSS::SIP::SIPMessage::Ptr& pMsg);
    /// Return a newly created SIPB2BClientTransaction object.
    /// Implementors may choose to override this and return their
    /// own custom SIPB2BClientTransaction subclass.


  const boost::filesystem::path& getTransportConfigurationFile() const;
    /// Returns the path of the transport configuration file.
    /// This configuration file will be used by the SIP stack
    /// to initialize its listeners

  SIPB2BHandler::Ptr findHandler(const OSS::SIP::SIPMessage::Ptr& pMsg) const;
    /// Returns the iterator for the request handler if one is registered

  SIPB2BHandler::Ptr findHandler(SIPB2BHandler::MessageType type) const;
    /// Returns the iterator for the request handler if one is registered

public:
  SIPB2BHandler::Ptr findDomainRouter(const std::string& domain) const;
  SIPB2BHandler::Ptr findDomainRouter(const OSS::SIP::SIPMessage::Ptr& pMsg) const;
    /// Returns the iterator for the domain router if one is registered
  
  virtual SIPMessage::Ptr postMidDialogTransactionCreated(
    const SIPMessage::Ptr& pRequest, SIPB2BTransaction::Ptr pTransaction);
    /// Called by handlers when a mid dialog trasaction has been created

  virtual bool postRetargetTransaction(
    SIPMessage::Ptr& pRequest,
    OSS::SIP::B2BUA::SIPB2BTransaction::Ptr pTransaction);
    //
    // This allows the application to execute a retarget prior to actual route scripts being called
    //

  void setExternalDispatch(const ExternalDispatch& externalDispatch);
    /// Allow the application to set it's own transaction dispatcher
 
  void addPendingSubscription(const std::string& callId);
  void removePendingSubscription(const std::string& callId);
  bool isSubscriptionPending(const std::string& callId) const;
  
  int getMaxThreadCount() const;
  
private:
  OSS::thread_pool _threadPool;
  OSS::mutex_critic_sec _csDialogsMutex;
  bool _useSourceAddressForResponses;
  MessageHandlers _handlers;
  DomainRouters _domainRouters;
  boost::filesystem::path _transportConfigurationFile;
  std::string _sipConfigFile;
  PostRouteCallback _postRouteCallback;
  std::string _userAgentName;
  SIPB2BHandler* _pDefaultHandler;
  ExternalDispatch _externalDispatch;
  std::set<std::string> _pendingSubscriptions;
  mutable OSS::mutex_critic_sec _pendingSubscriptionsMutex;
  int _maxThreadCount; 

  //
  // Plugins
  //
  SIPB2BUserAgentHandlerList _userAgentHandler;
  SIPB2BUserAgentHandlerLoader _pluginLoader;
   
#if ENABLE_FEATURE_LIBRE
  //
  // Local registration agent
  //
  SIPB2BRegisterAgent _registerAgent;
  std::string _registerAgentRoute;
#endif
  
};

//
// Inlines
//
inline OSS::thread_pool& SIPB2BTransactionManager::threadPool()
{
  return _threadPool;
}

inline bool& SIPB2BTransactionManager::useSourceAddressForResponses()
{
  return _useSourceAddressForResponses;
}

inline const boost::filesystem::path& SIPB2BTransactionManager::getTransportConfigurationFile() const
{
  return _transportConfigurationFile;
}

inline SIPB2BTransactionManager::MessageHandlers& SIPB2BTransactionManager::handlers()
{
  return _handlers;
}

inline void SIPB2BTransactionManager::setSipConfigFile(const std::string& sipConfigFile)
{
  _sipConfigFile = sipConfigFile;
}

inline void SIPB2BTransactionManager::setPostRouteCallback(const PostRouteCallback& postRouteCallback)
{
  _postRouteCallback = postRouteCallback;
}

inline const SIPB2BTransactionManager::PostRouteCallback& SIPB2BTransactionManager::getPostRouteCallback() const
{
  return _postRouteCallback;
}

inline void SIPB2BTransactionManager::registerDefaultHandler(SIPB2BHandler* pDefaultHandler)
{
  _pDefaultHandler = pDefaultHandler;
}

inline void SIPB2BTransactionManager::setExternalDispatch(const ExternalDispatch& externalDispatch)
{
  _externalDispatch = externalDispatch;;
}

inline int SIPB2BTransactionManager::getMaxThreadCount() const
{
  return _maxThreadCount;
}


} } } // OSS::SIP::B2BUA

#endif // ENABLE_FEATURE_B2BUA

#endif

