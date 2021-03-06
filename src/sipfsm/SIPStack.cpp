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


#include "OSS/SIP/SIPStack.h"
#include "OSS/OSS.h"
#include "OSS/SIP/SIPException.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPTransportSession.h"
#include "OSS/UTL/Logger.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>


#if ENABLE_FEATURE_CONFIG
using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;
using OSS::Persistent::PersistenceException;
#endif

namespace OSS {
namespace SIP {


SIPStack::SIPStack() :
  _fsmDispatch(),
  _enableUDP(true),
  _enableTCP(true),
#if ENABLE_FEATURE_WEBSOCKETS 
  _enableWS(true),
#endif
  _enableTLS(true),
  _udpListeners(),
  _tcpListeners(),
  _wsListeners(),
  _tlsListeners(),
  _tlsCertPassword()
{
}

SIPStack::~SIPStack()
{
}

void SIPStack::run()
{
  _fsmDispatch.transport().run();
}

void SIPStack::transportInit()
{
  bool hasUDP = _udpListeners.size() > 0;
  bool hasTCP = _tcpListeners.size() > 0;
  bool hasWS  = _wsListeners.size() > 0;
  bool hasTLS = _tlsListeners.size() > 0;
  
  if (!hasUDP && !hasTCP && !hasWS && !hasTLS)
    throw OSS::SIP::SIPException("No Listener Address Configured");

  //
  // Prepare the UDP Transport
  //
  if (_enableUDP)
  {
    for (std::size_t i = 0; i < _udpListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _udpListeners[i];
      std::string ip = iface.address().to_string();
      std::string port = OSS::string_from_number<unsigned long>(iface.getPort());
      
      std::string ipPort = iface.toIpPortString();
      SubNets::const_iterator subnetIter = _udpSubnets.find(ipPort);
      SIPListener::SubNets subnets;
      if (subnetIter != _udpSubnets.end())
        subnets = subnetIter->second;
      
      _fsmDispatch.transport().addUDPTransport(ip, port, iface.externalAddress(), subnets, iface.isVirtual(), iface.alias());
    }
  }

  //
  // Prepare the TCP Transport
  //
  if (_enableTCP)
  {
    for (std::size_t i = 0; i < _tcpListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _tcpListeners[i];
      std::string ip = iface.address().to_string();
      std::string port = OSS::string_from_number(iface.getPort());
      std::string ipPort = iface.toIpPortString();
      SubNets::const_iterator subnetIter = _tcpSubnets.find(ipPort);
      SIPListener::SubNets subnets;
      if (subnetIter != _tcpSubnets.end())
        subnets = subnetIter->second;
      
      _fsmDispatch.transport().addTCPTransport(ip, port, iface.externalAddress(), subnets, iface.isVirtual(), iface.alias());
    }
  }

#if ENABLE_FEATURE_WEBSOCKETS  
  //
  // Prepare the WebSocket Transport
  //
  if (_enableWS)
  {
    for (std::size_t i = 0; i < _wsListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _wsListeners[i];
      std::string ip = iface.address().to_string();
      std::string port = OSS::string_from_number(iface.getPort());
      std::string ipPort = iface.toIpPortString();
      SubNets::const_iterator subnetIter = _wsSubnets.find(ipPort);
      SIPListener::SubNets subnets;
      if (subnetIter != _wsSubnets.end())
        subnets = subnetIter->second;
      
      _fsmDispatch.transport().addWSTransport(ip, port, iface.externalAddress(), subnets, iface.isVirtual(), iface.alias());
    }
  }
#endif

  //
  // Prepare the TLS Transport
  //
  if (_enableTLS)
  {
    for (std::size_t i = 0; i < _tlsListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _tlsListeners[i];
      std::string ip = iface.address().to_string();
      std::string port = OSS::string_from_number(iface.getPort());
      std::string ipPort = iface.toIpPortString();
      SubNets::const_iterator subnetIter = _tlsSubnets.find(ipPort);
      SIPListener::SubNets subnets;
      if (subnetIter != _tlsSubnets.end())
        subnets = subnetIter->second;
      
      _fsmDispatch.transport().addTLSTransport(ip, port, iface.externalAddress(), subnets, iface.isVirtual(), iface.alias());
    }
  }
}

void SIPStack::transportInit(unsigned short udpPortBase, unsigned short udpPortMax,
    unsigned short tcpPortBase, unsigned short tcpPortMax,
    unsigned short wsPortBase, unsigned short wsPortMax,
    unsigned short tlsPortBase, unsigned short tlsPortMax)
{
  OSS_VERIFY(udpPortBase <= udpPortMax);
  OSS_VERIFY(tcpPortBase <= tcpPortMax);
  OSS_VERIFY(tlsPortBase <= tlsPortMax);

  bool hasUDP = _udpListeners.size() > 0;
  bool hasTCP = _tcpListeners.size() > 0;
  bool hasWS = _wsListeners.size() > 0;
  bool hasTLS = _tcpListeners.size() > 0;

  if (!hasUDP && !hasTCP && !hasWS && !hasTLS)
    throw OSS::SIP::SIPException("No Listener Address Configured");

  //
  // Reset to false so we could use it as success indicator
  //
  hasUDP = false;
  hasTCP = false;
  hasWS = false;
  hasTLS = false;

  //
  // Prepare the UDP Transport
  //
  if (_enableUDP)
  {
    for (std::size_t i = 0; i < _udpListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _udpListeners[i];
      std::string ip = iface.address().to_string();
      std::string ipPort = iface.toIpPortString();
      SubNets::const_iterator subnetIter = _udpSubnets.find(ipPort);
      SIPListener::SubNets subnets;
      if (subnetIter != _udpSubnets.end())
        subnets = subnetIter->second;
      
      for(unsigned short p = udpPortBase; p <= udpPortMax; p++)
      {
        try
        {
          std::string port = OSS::string_from_number<unsigned short>(p);
          _fsmDispatch.transport().addUDPTransport(ip, port, iface.externalAddress(), subnets, iface.isVirtual(), iface.alias());
          iface.setPort(p);
          hasUDP = true;
          break;
        }
        catch(...)
        {
          continue;
        }
      }
    }
  }

  //
  // Prepare the TCP Transport
  //
  if (_enableTCP)
  {
    for (std::size_t i = 0; i < _tcpListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _tcpListeners[i];
      std::string ip = iface.address().to_string();
      std::string ipPort = iface.toIpPortString();
      SubNets::const_iterator subnetIter = _tcpSubnets.find(ipPort);
      SIPListener::SubNets subnets;
      if (subnetIter != _tcpSubnets.end())
        subnets = subnetIter->second;
      
      for(unsigned short p = tcpPortBase; p <= tcpPortMax; p++)
      {
        try
        {
          std::string port = OSS::string_from_number<unsigned short>(p);
          _fsmDispatch.transport().addTCPTransport(ip, port,iface.externalAddress(), subnets, iface.isVirtual(), iface.alias());
          iface.setPort(p);
          hasTCP = true;
          break;
        }
        catch(...)
        {
          continue;
        }
      }
    }
  }

#if ENABLE_FEATURE_WEBSOCKETS
  //
  // Prepare the WebSocket Transport
  //
  if (_enableWS)
  {
    for (std::size_t i = 0; i < _wsListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _wsListeners[i];
      std::string ip = iface.address().to_string();
      std::string ipPort = iface.toIpPortString();
      SubNets::const_iterator subnetIter = _wsSubnets.find(ipPort);
      SIPListener::SubNets subnets;
      if (subnetIter != _wsSubnets.end())
        subnets = subnetIter->second;
      
      for(unsigned short p = wsPortBase; p <= wsPortMax; p++)
      {
        try
        {
          std::string port = OSS::string_from_number<unsigned short>(p);
          _fsmDispatch.transport().addWSTransport(ip, port,iface.externalAddress(), subnets, iface.isVirtual(), iface.alias());
          iface.setPort(p);
          hasWS = true;
          break;
        }
        catch(...)
        {
          continue;
        }
      }
    }
  }
#endif

  //
  // Prepare the TLS Transport
  //
  if (_enableTLS)
  {
    for (std::size_t i = 0; i < _tlsListeners.size(); i++)
    {
      OSS::Net::IPAddress& iface = _tlsListeners[i];
      std::string ip = iface.address().to_string();
      std::string ipPort = iface.toIpPortString();
      SubNets::const_iterator subnetIter = _tlsSubnets.find(ipPort);
      SIPListener::SubNets subnets;
      if (subnetIter != _tlsSubnets.end())
        subnets = subnetIter->second;
      
      for(unsigned short p = tlsPortBase; p <= tlsPortMax; p++)
      {
        try
        {
          std::string port = OSS::string_from_number<unsigned short>(p);
          _fsmDispatch.transport().addTLSTransport(ip, port, iface.externalAddress(), subnets, iface.isVirtual(), iface.alias());
          iface.setPort(p);
          hasTLS = true;
          break;
        }
        catch(...)
        {
          continue;
        }
      }
    }
  }

  if (!hasUDP && !hasTCP && !hasWS && !hasTLS)
    throw OSS::SIP::SIPException("No Listener Address Configured");
}


#if ENABLE_FEATURE_CONFIG
bool SIPStack::initVirtualTransportFromConfig(const boost::filesystem::path& cfgFile)
{
  ClassType configFile;
  
  if (!configFile.load(OSS::boost_path(cfgFile)))
  {
    OSS_LOG_ERROR("SIPStack::initVirtualTransportFromConfig() - Unable to open config file " << cfgFile);
    return false;
  }
  
  DataType root = configFile.self();
  
  if (!root.exists("listeners"))
  {
    OSS_LOG_ERROR("SIPStack::initVirtualTransportFromConfig() - Section [listeners] does not exist");
    return false;
  }
  DataType listeners = root["listeners"];
  
  if (!listeners.exists("carp-ha-interface"))
  {
    OSS_LOG_NOTICE("SIPStack::initVirtualTransportFromConfig() - Section [carp-ha-interface] does not exist");
    return false;
  }
  
  DataType carpConfig = listeners["carp-ha-interface"];
  
  if (!carpConfig.getElementCount())
  {
    OSS_LOG_ERROR("SIPStack::initVirtualTransportFromConfig() - Section [carp-ha-interface] has empty elements");
    return false;
  }
  //
  // Check if CARP is enabled
  //
  bool isEnabled = carpConfig[0].exists("enabled") && (bool)carpConfig[0]["enabled"];
  if (!isEnabled)
  {
    OSS_LOG_NOTICE("SIPStack::initVirtualTransportFromConfig() - Carp is not enabled");
    return false;
  }
  
  /*
   
   *  virtual-ip-address = "0.0.0.0/0"; // Virtual IP address and subnet to be used by CARP
      external-address = "0.0.0.0";  // External IP address if the virtual IP is behind a port mapped firewall
      up-script = "/etc/karoo.conf.d/ucarp-vip-up.sh";  // Script executed to register the virtual IP
      down-script = "/etc/karoo.conf.d/ucarp-vip-down.sh"; // Script executed to remove the virtual IP
      carp-password = "DesertForbiddenFruit"; // Secret key used by CARP subsystem
      preferred-master = false; // Set this to true if you want to make this instance as the preferred master
      tcp-enabled = true;  // Enable TCP transport
      udp-enabled = true; // Enable UDP Transport
      ws-enabled = true; // Enable WebSocket Transport
      tls-enabled = true; // Enable TLS Transport
      sip-port = 5060; // Port for TCP and UDP
      tls-port = 5061; // Port for TLS
      ws-port = 5062; // Port for WebSocket
      subnets = "0.0.0.0/0"; // (Optional) Comma delimited subnets reachable by this transport
   */
  
  //
  // Determine Virtual IP and subnet
  //
  if (!carpConfig[0].exists("virtual-ip-address"))
  {
    OSS_LOG_ERROR("SIPStack::initVirtualTransportFromConfig() - Property [virtual-ip-address] is not set.  Carp will be disabled.");
    return false;
  }
  
  std::string virtualIp((const char*)carpConfig[0]["virtual-ip-address"]);
  
  //
  // Check if the virtual IP specified a subnet
  //
  std::vector<std::string> vipTokens = OSS::string_tokenize(virtualIp, "/");
  if (vipTokens.size() == 2)
  {
    virtualIp = vipTokens[0];
  }
  
  //
  // Determine subnets
  //
  std::vector<std::string> subnets;
  if (carpConfig[0].exists("subnets"))
  {
    std::string strSubnet((const char*)carpConfig[0]["subnets"]);
    subnets = OSS::string_tokenize(strSubnet, ",");
  }
  
  //
  // Determine External IP Address
  //
  std::string externalAddress = virtualIp;
  if (carpConfig[0].exists("external-address"))
  {
    std::string extAddr = (const char*)carpConfig[0]["external-address"];
    if (extAddr != "0.0.0.0")
      externalAddress = extAddr;
  }
  
  bool tcpEnabled = carpConfig[0].exists("tcp-enabled") && (bool)carpConfig[0]["tcp-enabled"];
  bool udpEnabled = carpConfig[0].exists("udp-enabled") && (bool)carpConfig[0]["udp-enabled"];
  bool tlsEnabled = carpConfig[0].exists("tls-enabled") && (bool)carpConfig[0]["tls-enabled"];
  bool wsEnabled = carpConfig[0].exists("ws-enabled") && (bool)carpConfig[0]["ws-enabled"];
  
  if (!tcpEnabled && !udpEnabled && !tlsEnabled && !wsEnabled)
  {
    //
    // Let us default to UDP and TCP
    //
    tcpEnabled = true;
    udpEnabled = true;
  }
  
  OSS::Net::IPAddress listenerAddress(virtualIp);
  listenerAddress.externalAddress() = externalAddress;
  listenerAddress.setVirtual(true);
  
  if (udpEnabled)
  {
    int port = 5060;
    if (carpConfig[0].exists("sip-port"))
    {
      port = (int)carpConfig[0]["sip-port"];
    }
    listenerAddress.setPort(port);
    
    _udpListeners.push_back(listenerAddress);
    _udpSubnets[listenerAddress.toIpPortString()] = subnets;
  }
  
  if (tcpEnabled)
  {
    int port = 5060;
    if (carpConfig[0].exists("sip-port"))
    {
      port = (int)carpConfig[0]["sip-port"];
    }
    listenerAddress.setPort(port);
    
    _tcpListeners.push_back(listenerAddress);
    _tcpSubnets[listenerAddress.toIpPortString()] = subnets;
  }
  
  if (tlsEnabled)
  {   
    int port = 5061;
    if (carpConfig[0].exists("tls-port"))
    {
      port = (int)carpConfig[0]["tls-port"];
    }
    listenerAddress.setPort(port);
    
    _tlsListeners.push_back(listenerAddress);
    _tlsSubnets[listenerAddress.toIpPortString()] = subnets;
  }
  
  if (wsEnabled)
  {   
    int port = 5062;
    if (carpConfig[0].exists("ws-port"))
    {
      port = (int)carpConfig[0]["ws-port"];
    }
    listenerAddress.setPort(port);
    
    _wsListeners.push_back(listenerAddress);
    _wsSubnets[listenerAddress.toIpPortString()] = subnets;
  }
  
  return true;
}

bool SIPStack::initTlsContextFromConfig(const boost::filesystem::path& cfgFile)
{
  ClassType config;
  config.load(OSS::boost_path(cfgFile));
  DataType root = config.self();
  DataType listeners = root["listeners"];
  
 /****************************************************************************
  * The TLS Certificate Authority                                            *
  ****************************************************************************/
  std::string tls_ca_file;
  if (listeners.exists("tls-ca-file"))
  {
    tls_ca_file = (const char*)listeners["tls-ca-file"];
  }

 /****************************************************************************
  * The Additional TLS Certificate Authority Directory.                      *
  * The files must be named with the CA subject name hash value.             *
  * (see man SSL_CTX_load_verify_locations for more info)                    *
  ****************************************************************************/
  std::string tls_ca_path;
  if (listeners.exists("tls-ca-path"))
  {
    tls_ca_path = (const char*)listeners["tls-ca-path"];
  }

 /****************************************************************************
  * The TLS Server Certificate                                               *
  ****************************************************************************/
  std::string tls_certificate_file;
  if (listeners.exists("tls-certificate-file"))
  {
    tls_certificate_file = (const char*)listeners["tls-certificate-file"];
  }

 /****************************************************************************
  * The TLS Server Private Key                                               *
  ****************************************************************************/
  std::string tls_private_key_file;
  if (listeners.exists("tls-private-key-file"))
  {
    tls_private_key_file = (const char*)listeners["tls-private-key-file"];
  }

 /****************************************************************************
  * The TLS Certificate Password                                             *
  ****************************************************************************/
  std::string tls_cert_password;
  if (listeners.exists("tls-cert-password"))
  {
    tls_cert_password = (const char*)listeners["tls-cert-password"];
  }

 /****************************************************************************
  * The TLS Verification Mode.  True if server will verify client certs      *
  ****************************************************************************/
  bool tls_verify_peer = false;
  if (listeners.exists("tls-verify-peer"))
  {
    tls_verify_peer = (bool)listeners["tls-verify-peer"];
  }
  
  return initializeTlsContext(tls_certificate_file, tls_private_key_file, tls_cert_password, tls_ca_file, tls_ca_path, tls_verify_peer);
}

#endif // ENABLE_FEATURE_CONFIG

void SIPStack::setTransportThreshold(
  unsigned long packetsPerSecondThreshold, // The total packets per second threshold
  unsigned long thresholdViolationRate, // Per IP threshold
  int banLifeTime // violator jail lifetime
)
{
  if (packetsPerSecondThreshold > thresholdViolationRate)
  {
    SIPTransportSession::rateLimit().enabled() = true;
    SIPTransportSession::rateLimit().autoBanThresholdViolators() = true;
    SIPTransportSession::rateLimit().setPacketsPerSecondThreshold(packetsPerSecondThreshold);
    SIPTransportSession::rateLimit().setThresholdViolationRate(thresholdViolationRate);
    SIPTransportSession::rateLimit().setBanLifeTime(banLifeTime);    
    OSS_LOG_INFO("Enforcing packet rate limit = " << thresholdViolationRate);
  }
}

#if ENABLE_FEATURE_CONFIG

static void registerConfiguredTransport(OSS::socket_address_list& listeners,
  SIPStack::SubNets& subnetMap,
  const std::string ip, 
  int port, 
  const std::string& external, 
  const std::vector<std::string>& subnets)
{
  const std::vector<OSS::Net::IPAddress>& localAddresses = OSS::Net::IPAddress::getLocalAddresses();
  OSS::Net::IPAddress listener;
  listener = ip;

  if (!listener.isInaddrAny())
  {
    listener.externalAddress() = external;
    listener.setPort(port);
    listeners.push_back(listener);
    subnetMap[listener.toIpPortString()] = subnets;
  }
  else
  {
    for(std::vector<OSS::Net::IPAddress>::const_iterator iter = localAddresses.begin(); iter != localAddresses.end(); iter++)
    {
      //
      // We only support V4 for now
      //
      OSS::Net::IPAddress localAddr(*iter);
      if (localAddr.address().is_v4())
      {
        localAddr.setPort(port);
        listeners.push_back(localAddr);
      }
    }
  }
}

void SIPStack::initTransportFromConfig(const boost::filesystem::path& cfgFile)
{
  ClassType config;
  config.load(OSS::boost_path(cfgFile));
  DataType root = config.self();
  DataType listeners = root["listeners"];
  
  //
  // Initialize TLS context
  //
  bool hasInitializedTLS = initTlsContextFromConfig(cfgFile);

  //
  // Initialize CARP virtual interfaces
  //
  bool hasVirtualInterfaces = initVirtualTransportFromConfig(cfgFile);
  
  if (hasVirtualInterfaces)
  {
    OSS_LOG_INFO("SIPStack::initTransportFromConfig() - Virtual Interface Added");
  }
  
  DataType interfaces = listeners["interfaces"];
  int ifaceCount = interfaces.getElementCount();
  bool hasFoundDefault = false;
  std::string defaultAddress;
  if (OSS::Net::IPAddress::getDefaultAddress().isValid())
  {
    defaultAddress = OSS::Net::IPAddress::getDefaultAddress().toString();
  }
  
  for (int i = 0; i < ifaceCount; i++)
  {
    DataType iface = interfaces[i];
    std::string ip = iface["ip-address"];
    std::string external;
    if (iface.exists("external-address"))
    {
      external = (const char*)iface["external-address"];
    }

    if ((ip == "auto" || ip == "AUTO") && defaultAddress.empty())
    {
      OSS_LOG_ERROR("SIPStack::initTransportFromConfig - unable to determine default interface address");
      return;
    }
    else if (ip == "auto" || ip == "AUTO")
    {
      ip = defaultAddress;
      OSS_LOG_NOTICE("SIPStack::initTransportFromConfig - using default address " << defaultAddress);
    }
    
    bool tlsEnabled = iface.exists("tls-enabled") && (bool)iface["tls-enabled"];
    bool tcpEnabled = iface.exists("tcp-enabled") && (bool)iface["tcp-enabled"];
    bool wsEnabled = iface.exists("ws-enabled") && (bool)iface["ws-enabled"];
    
    bool udpEnabled = true;
    if (iface.exists("udp-enabled"))
      udpEnabled = (bool)iface["udp-enabled"];

    int sipPort = iface.exists("sip-port") ?  (int)iface["sip-port"] : 5060;
    int tlsPort = iface.exists("tls-port") ?  (int)iface["tls-port"] : 5061;
    int wsPort = iface.exists("ws-port") ?  (int)iface["ws-port"] : 5062;

    std::vector<std::string> subnets;
    if (iface.exists("subnets"))
    {
      std::string strSubnet((const char*)iface["subnets"]);
      subnets = OSS::string_tokenize(strSubnet, ",");
    }
    
    if (!hasFoundDefault)
    {
      if (iface.exists("default"))
      {
        hasFoundDefault = ((bool)iface["default"]);
        bool transportEnabled = udpEnabled || tcpEnabled || wsEnabled || tlsEnabled;

        if (hasFoundDefault  && transportEnabled)
        {
          OSS::Net::IPAddress listener;
          listener = ip;
          if (listener.isInaddrAny())
          {
            hasFoundDefault = false;
          }
          else
          {
            listener.externalAddress() = external;
            listener.setPort(sipPort);
            _fsmDispatch.transport().defaultListenerAddress() = listener;
          }
        }
      }
    }

    if (udpEnabled)
    {
      registerConfiguredTransport(_udpListeners, _udpSubnets, ip, sipPort, external, subnets);
    }

    if (tcpEnabled)
    {
      registerConfiguredTransport(_tcpListeners, _tcpSubnets, ip, sipPort, external, subnets);
    }

    if (wsEnabled)
    {
      registerConfiguredTransport(_wsListeners, _wsSubnets, ip, wsPort, external, subnets);
    }

    if (tlsEnabled && hasInitializedTLS)
    {      
      registerConfiguredTransport(_tlsListeners, _tlsSubnets, ip, tlsPort, external, subnets);
    }
  }

  if (!hasFoundDefault)
  {
    //
    // Set the default interface for the transport service the old fashioned way
    //
    if (listeners.exists("default-interface-address") &&
      listeners.exists("default-interface-port"))
    {
      hasFoundDefault = true;
      DataType defaultIface = listeners["default-interface-address"];
      DataType defaultPort = listeners["default-interface-port"];
      
      std::string ip = (const char*)defaultIface;
      
      if ((ip == "auto" || ip == "AUTO") && defaultAddress.empty())
      {
        OSS_LOG_ERROR("SIPStack::initTransportFromConfig - unable to determine default interface address");
        return;
      }
      else if (ip == "auto" || ip == "AUTO")
      {
        ip = defaultAddress;
      }
      
      OSS::Net::IPAddress defaultInterface(ip);
      if (!defaultInterface.isValid())
      {
        OSS_LOG_ERROR("SIPStack::initTransportFromConfig - unable to determine default interface address");
        return;
      }
      defaultInterface.setPort((int)defaultPort);
      _fsmDispatch.transport().defaultListenerAddress() = defaultInterface;
    }
  }

  if (!hasFoundDefault && ifaceCount > 0)
  {
    //
    // We don't have the defualt interface yet.  Lets use the first configured listener
    //
    DataType iface = interfaces[0];
    std::string ip = (const char*)iface["ip-address"];
    if ((ip == "auto" || ip == "AUTO") && defaultAddress.empty())
    {
      OSS_LOG_ERROR("SIPStack::initTransportFromConfig - unable to determine default interface address");
      return;
    }
    else if (ip == "auto" || ip == "AUTO")
    {
      ip = defaultAddress;
    }
    
    if (iface.exists("udp-enabled") && (bool)iface["udp-enabled"])
    {
      int port = iface.exists("sip-port") ? (int)iface["sip-port"] : 5060;
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.setPort(port);
      _fsmDispatch.transport().defaultListenerAddress() = listener;
    }
    else if (iface.exists("tcp-enabled") && (bool)iface["tcp-enabled"])
    {
      int port = iface.exists("sip-port") ? (int)iface["sip-port"] : 5060;
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.setPort(port);
      _fsmDispatch.transport().defaultListenerAddress() = listener;
    }
    else if (iface.exists("ws-enabled") && (bool)iface["ws-enabled"])
    {
      int port = iface.exists("sip-port") ? (int)iface["sip-port"] : 5060;
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.setPort(port);
      _fsmDispatch.transport().defaultListenerAddress() = listener;
    }
    else if (iface.exists("tls-enabled") && (bool)iface["tls-enabled"])
    {
      int port = iface.exists("tls-port") ? (int)iface["tsl-port"] : 5061;
      OSS::Net::IPAddress listener;
      listener = ip;
      listener.setPort(port);
      _fsmDispatch.transport().defaultListenerAddress() = listener;
    }
  }

  //
  // Set the TCP port range
  //
  if (listeners.exists("sip-tcp-port-base") && listeners.exists("sip-tcp-port-max"))
  {
    unsigned int tcpPortBase = listeners["sip-tcp-port-base"];
    unsigned int tcpPortMax = listeners["sip-tcp-port-max"];
    if (tcpPortBase < tcpPortMax && tcpPortBase > 0)
    {
      OSS_LOG_INFO("Setting TCP port range to " << tcpPortBase << "-" << tcpPortMax);
      transport().setTCPPortRange((unsigned short)tcpPortBase, (unsigned short)tcpPortMax);
    }
    else
    {
      OSS_LOG_ERROR("Unable to set TCP port base " << tcpPortBase << "-" << tcpPortMax << " Using default values.");
    }
  }

  //
  // Set the WS port range
  //
#if ENABLE_FEATURE_WEBSOCKETS
  if (listeners.exists("sip-ws-port-base") && listeners.exists("sip-ws-port-max"))
  {
    unsigned int wsPortBase = listeners["sip-ws-port-base"];
    unsigned int wsPortMax = listeners["sip-ws-port-max"];
    if (wsPortBase < wsPortMax && wsPortBase > 0)
    {
      OSS_LOG_INFO("Setting WebSocket port range to " << wsPortBase << "-" << wsPortMax);
      transport().setWSPortRange((unsigned short)wsPortBase, (unsigned short)wsPortMax);
    }
    else
    {
      OSS_LOG_ERROR("Unable to set WebSocket port base " << wsPortBase << "-" << wsPortMax << " Using default values.");
    }
  }
#endif

  if (listeners.exists("packet-rate-ratio"))
  {
    std::string packetRateRatio = (const char*)listeners["packet-rate-ratio"];
    std::vector<std::string> tokens = OSS::string_tokenize(packetRateRatio, "/");
    if (tokens.size() == 3)
    {
      unsigned long packetsPerSecondThreshold;
      unsigned long thresholdViolationRate;
      int banLifeTime;
      thresholdViolationRate = OSS::string_to_number<unsigned long>(tokens[0].c_str());
      packetsPerSecondThreshold = OSS::string_to_number<unsigned long>(tokens[1].c_str());
      banLifeTime = OSS::string_to_number<int>(tokens[2].c_str());

      setTransportThreshold(packetsPerSecondThreshold, thresholdViolationRate, banLifeTime);
    
      if (listeners.exists("packet-rate-white-list"))
      {
        DataType whiteList = listeners["packet-rate-white-list"];
        int count = whiteList.getElementCount();
        for (int i = 0; i < count; i++)
        {
          DataType wl = whiteList[i];
          std::string entry;
          if (wl.exists("source-ip"))
          {
            entry = (const char*)wl["source-ip"];
            if (!entry.empty())
            {
              boost::system::error_code ec;
              boost::asio::ip::address ip = boost::asio::ip::address::from_string(entry, ec);
              if (!ec)
                SIPTransportSession::rateLimit().whiteListAddress(ip);
            }
          }
          else if (wl.exists("source-network"))
          {
            entry = (const char*)wl["source-network"];
            if (!entry.empty())
            SIPTransportSession::rateLimit().whiteListNetwork(entry);
          }
        }
      }
    }
  }
  
  if (listeners.exists("auto-null-route-on-ban"))
  {
    bool autoNullRouteOnBan = (bool)listeners["auto-null-route-on-ban"];
    if (autoNullRouteOnBan)
    {
      SIPTransportSession::rateLimit().setAutoNullRoute(true);
    }
  }

  transportInit();
}

#endif // ENABLE_FEATURE_CONFIG

bool SIPStack::initializeTlsContext(
    const std::string& tlsCertFile, // Certificate to be used by this server.  File should be in PEM format
    const std::string& privateKey, // Private key to be used by this server.  File should be in PEM format
    const std::string& tlsCertFilePassword, // Set this value if tlsCertFile is password protected
    const std::string& peerCaFile, // If the remote peer this server is connecting to uses a self signed certificate, this file is used to verify authenticity of the peer identity
    const std::string& peerCaPath, // A directory full of CA certificates. The files must be named with the CA subject name hash value. (see man SSL_CTX_load_verify_locations for more info)
    bool verifyPeer // Verify the peer certificates.  If the peer CA file is not set, set this value to false
)
{
  boost::asio::ssl::context& tlsClientContext = transport().tlsClientContext();
  boost::asio::ssl::context& tlsServerContext = transport().tlsServerContext();
  //
  // configure the client context
  //
  if (verifyPeer)
  {
    tlsClientContext.set_verify_mode(boost::asio::ssl::context::verify_none);
    tlsServerContext.set_verify_mode(boost::asio::ssl::context::verify_peer | boost::asio::ssl::context::verify_fail_if_no_peer_cert);
    OSS_LOG_INFO("SIPStack::initializeTlsContext - Peer Certificate Verification Enforcing");
  }
  else
  {
    tlsClientContext.set_verify_mode(boost::asio::ssl::context::verify_none);
    tlsServerContext.set_verify_mode(boost::asio::ssl::context::verify_none);
    OSS_LOG_INFO("SIPStack::initializeTlsContext - Peer Certificate Verification Disabled");
  }
  
  if (!peerCaFile.empty())
  {
    try
    {
      tlsClientContext.load_verify_file(peerCaFile);
      tlsServerContext.load_verify_file(peerCaFile);
    }
    catch(...)
    {
      OSS_LOG_WARNING("SIPStack::initializeTlsContext - Unable to load peerCaFile " << peerCaFile << " TLS will be disabled.");
      return false;
    }
    OSS_LOG_INFO("SIPStack::initializeTlsContext - Loaded peerCaFile " << peerCaFile);
  }

  if (!peerCaPath.empty())
  {
    try
    {
      tlsClientContext.add_verify_path(peerCaPath);
      tlsServerContext.add_verify_path(peerCaPath);
    }
    catch(...)
    {
      OSS_LOG_ERROR("SIPStack::initializeTlsContext - Unable to add peerCaPath " << peerCaPath);
      return false;
    }
    OSS_LOG_INFO("SIPStack::initializeTlsContext - Loaded peerCaPath " << peerCaPath);
  }
  
  //
  // Configure the server context
  //
  
  _tlsCertPassword = tlsCertFilePassword;
  
  try
  {
    tlsClientContext.set_password_callback(boost::bind(&SIPStack::getTlsCertPassword, this));
    tlsClientContext.use_certificate_file(tlsCertFile, boost::asio::ssl::context::pem);
    tlsClientContext.use_private_key_file(privateKey, boost::asio::ssl::context::pem);
    
    tlsServerContext.set_password_callback(boost::bind(&SIPStack::getTlsCertPassword, this));
    tlsServerContext.use_certificate_file(tlsCertFile, boost::asio::ssl::context::pem);
    tlsServerContext.use_private_key_file(privateKey, boost::asio::ssl::context::pem);
  }
  catch(...)
  {
    OSS_LOG_ERROR("SIPStack::initializeTlsContext - Unable to add tlsCertFile " << tlsCertFile);
    return false;
  }
  
  OSS_LOG_INFO("SIPStack::initializeTlsContext - Server certificate " << tlsCertFile << " loaded");
  
  return true;
}

void SIPStack::stop()
{
  _fsmDispatch.stop();
  _fsmDispatch.transport().stop();
}

void SIPStack::sendRequest(
  const SIPMessage::Ptr& pRequest,
  const OSS::Net::IPAddress& localAddress,
  const OSS::Net::IPAddress& remoteAddress,
  SIPTransaction::Callback callback,
  SIPTransaction::TerminateCallback terminateCallback)
{
  _fsmDispatch.sendRequest(pRequest, localAddress, remoteAddress, callback, terminateCallback);
}

void SIPStack::sendRequestDirect(const SIPMessage::Ptr& pRequest,
  const OSS::Net::IPAddress& localAddress,
  const OSS::Net::IPAddress& remoteAddress)
{

  std::string logId = pRequest->createContextId(true);
  std::string transport;
  if (SIPVia::msgGetTopViaTransport(pRequest.get(), transport))
  {
    std::string transportId;
    pRequest->getProperty(OSS::PropertyMap::PROP_TransportId, transportId);
    if (transportId.empty())
      transportId="0";
    OSS_LOG_DEBUG(logId << "Sending request directly protocol=" << transport << " id=" << transportId);
    SIPTransportSession::Ptr client = _fsmDispatch.transport().createClientTransport(pRequest, localAddress, remoteAddress, transport, transportId);
    if (client) 
    {
      std::string isXOREncrypted = "0";
      pRequest->getProperty("xor", isXOREncrypted);

      std::ostringstream logMsg;
      logMsg << logId << ">>> " << pRequest->startLine()
      << " LEN: " << pRequest->data().size()
      << " SRC: " << localAddress.toIpPortString()
      << " DST: " << remoteAddress.toIpPortString()
      << " ENC: " << isXOREncrypted;
      OSS::log_information(logMsg.str());
      if (OSS::log_get_level() >= OSS::PRIO_DEBUG)
        OSS::log_debug(pRequest->createLoggerData());
      client->writeMessage(pRequest, remoteAddress.toString(), OSS::string_from_number(remoteAddress.getPort()));
    }
    else
      OSS_LOG_ERROR("SIPStack::sendRequestDirect failed - Unable to create client transport");
  }
  else
  {
    OSS_LOG_ERROR("SIPStack::sendRequestDirect failed - Unable to determine transport protocol.")
  }
}





} } // OSS::SIP

