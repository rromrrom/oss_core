
function RouteProfile()
{
  
}

RouteProfile.prototype.setRequest = function(request)
{
  this.sipMessage = new SIPMessage(request);
}

RouteProfile.prototype.isRoutable = function ()
{
  return false;
}

RouteProfile.prototype.routeRequest = function()
{
  this.setTransactionTimeout("6400");
  this.routeAccept();
}

RouteProfile.prototype.routeByAOR = function(userComparisonOnly)
{
  return this.sipMessage.routeByAOR(userComparisonOnly);
}

RouteProfile.prototype.setInterfaceAddress = function (iface, port)
{
  this.sipMessage.setProperty("interface-address", iface);
  this.sipMessage.setProperty("interface-port", port);
}

RouteProfile.prototype.setTargetAddress = function(transport, targetHost, targetPort)
{
  this.sipMessage.setProperty("target-transport", transport);
  this.sipMessage.setProperty("target-address", targetHost);
  this.sipMessage.setProperty("target-port", targetPort);
}

RouteProfile.prototype.setTargetDomain = function(targetDomain)
{
  this.sipMessage.setRequestUriHostPort(targetDomain);
  this.sipMessage.setFromHostPort(targetDomain);
  this.sipMessage.setToHostPort(targetDomain);
}

RouteProfile.prototype.setTargetUser = function(user)
{
  this.sipMessage.setRequestUriUser(user);
  this.sipMessage.setToUser(user);
}

RouteProfile.prototype.setTransactionTimeout = function(timeout)
{
  this.sipMessage.setProperty("transaction-timeout", timeout);
}

RouteProfile.prototype.flagRoute = function(routeAlias)
{
  this.sipMessage.setTransactionProperty(routeAlias, "1");
}

RouteProfile.prototype.isRouteFlagged = function(routeAlias)
{
  return  this.sipMessage.getTransactionProperty(routeAlias) == "1";
}

RouteProfile.prototype.allowFailover = function()
{
  this.sipMessage.setTransactionProperty("allow-failover", "1");
}

RouteProfile.prototype.disallowFailover = function()
{
  this.sipMessage.setTransactionProperty("allow-failover", "0");
}

RouteProfile.prototype.invokeLocalHandler = function()
{
  this.sipMessage.setTransactionProperty("invoke-local-handler", "1");
}

RouteProfile.prototype.allowRTPProxy = function()
{
  this.sipMessage.setTransactionProperty("no-rtp-proxy", "0");
}

RouteProfile.prototype.disallowRTPProxy = function()
{
  this.sipMessage.setTransactionProperty("no-rtp-proxy", "1");
}

RouteProfile.prototype.enableVerboseRTP = function()
{
  this.sipMessage.setTransactionProperty("enable-verbose-rtp", "1");
}

RouteProfile.prototype.enableLocal100Rel = function()
{
  this.sipMessage.setTransactionProperty("local-100-rel", "1");
}

RouteProfile.prototype.enableLocalUpdate = function()
{
  this.sipMessage.setTransactionProperty("local-update", "1");
}

RouteProfile.prototype.disbleVerboseRTP = function()
{
  this.sipMessage.setTransactionProperty("enable-verbose-rtp", "0");
}

RouteProfile.prototype.generateLocalResponse = function(yes)
{
  if (typeof yes == "undefined")
    yes = "1";
  this.sipMessage.setTrasanctionProperty("generate-local-response", yes);
}

RouteProfile.prototype.handleLocalReg = function(a1, realm, expires)
{
  if (!this.sipMessage.isRequest("REGISTER"))
    return;
  this.sipMessage.setTransactionProperty("invoke-local-handler", "1");
  this.sipMessage.setTransactionProperty("auth-a1-hash", a1);
  this.sipMessage.setTransactionProperty("auth-realm", realm);
  this.sipMessage.setTransactionProperty("reg-expires", expires);
}

RouteProfile.prototype.stringStartsWith = function(str1, str2)
{
  return (str1.substr(0, str2.length) == str2);
}

RouteProfile.prototype.removeStringPrefix = function(oldString, prefix)
{
  if (typeof oldString == "undefined" || typeof prefix == "undefined")
  {
    return "";
  }

  if (prefix.length > oldString.length)
    return oldString;

  if (oldString.substr(0, prefix.length) == prefix)
  {
    return oldString.substr(prefix.length, oldString.length - prefix.length);
  }

  return oldString;
}

RouteProfile.prototype.stripCallPrefix = function(prefix)
{
  var dialString = thissipMessage.getRequestUriUser();
  if (typeof dialString == "undefined" || typeof prefix == "undefined")
  {
    return;
  }
  var newDialString = this.removeStringPrefix(dialString, prefix);
  if (dialString.length > newDialString.length)
    this.sipMessage.setRequestUriUser(newDialString);
}

RouteProfile.prototype.routeReject = function()
{
  this.sipMessage.setProperty("route-action", "reject");
}

RouteProfile.prototype.routeAccept = function()
{
  this.sipMessage.setProperty("route-action", "continue");
}

RouteProfile.prototype.banSourceAddress = function()
{
  this.sipMessage.setProperty("route-action", "banned");
}

RouteProfile.prototype.setRejectReason = function(reason)
{
  this.sipMessage.setProperty("reject-reason", reason);
}

RouteProfile.prototype.setRejectCode = function(code)
{
  this.sipMessage.setProperty("reject-code", code);
}

RouteProfile.prototype.createA1Hash = function(userName, domain, password)
{
  return md5Hash(userName + ":" + domain + ":" + password);
}

