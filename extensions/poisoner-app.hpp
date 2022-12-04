#ifndef PoisonerAPP_H
#define PoisonerAPP_H

#include "ns3/ndnSIM-module.h"
#include "ns3/integer.h"
#include "ns3/string.h"
#include "ns3/nstime.h"

#include <ndn-cxx/security/key-chain.hpp> //for later use...

//from ndn-app.hpp, we get ndn-commmon, ndn-app-link-service, face, application, ptr, callback, and traced callback
#include "ns3/ndnSIM/apps/ndn-app.hpp"

namespace ns3{
namespace ndn{

//pretend App that calls on Poisoner to generate packets

//extending the ndn-app class to create my producer apps...
class PoisonerApp : public App
{
   public:
     static TypeId
     GetTypeId();

     PoisonerApp();//the constructor

     virtual void
     OnInterest(shared_ptr<const Interest> interest);

   protected:
     //inherited from application (overriding them to do more than just app stuff)
     virtual void
     StartApplication();

     virtual void
     StopApplication();

   private:
     //data packet production details
     Name m_prefix; //my prefix
     Time m_freshness;
     Name m_keyLocator;
     uint32_t m_signature;
     uint32_t m_payloadSize;

     //from ndn_app, I auto get: m_face, m_active, and logging details of traced callback
};

}//namesapace ndn

}//namespace ns3

#endif