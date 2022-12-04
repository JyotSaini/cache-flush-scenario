
#include "poisoner-app.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/ndnSIM/model/ndn-l3-protocol.hpp" //it's an L...
#include "helper/ndn-fib-helper.hpp"
#include "ns3/ndnSIM/ndn-cxx/name.hpp" //for name component creation...
#include <memory>

NS_LOG_COMPONENT_DEFINE("ndn.PoisonerApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(PoisonerApp);

TypeId
PoisonerApp::GetTypeId(void)
{
   //currently signature produced is default and keylocator is default (no methods to set them yet)
   static TypeId typeId = TypeId("ns3::ndn::PoisonerApp")
    .SetGroupName("Ndn")
    .SetParent<App>()
    .AddConstructor<PoisonerApp>()
    .AddAttribute("Prefix", "Prefix for evil producer interests", StringValue("/"),
                   MakeNameAccessor(&PoisonerApp::m_prefix), MakeNameChecker())
    .AddAttribute("PayloadSize", "Payload size for Content packets", UintegerValue(512), 
                  MakeUintegerAccessor(&PoisonerApp::m_payloadSize),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute("Freshness", "Freshness of the data packets", TimeValue(Seconds(0)), 
                  MakeTimeAccessor(&PoisonerApp::m_freshness), MakeTimeChecker())
    .AddAttribute("Signature", "Bad Signature that won't verify", UintegerValue(0), 
                  MakeUintegerAccessor(&PoisonerApp::m_signature), 
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute("KeyLocator", "Name for the Key Locator", NameValue(), 
                  MakeNameAccessor(&PoisonerApp::m_keyLocator), MakeNameChecker());

   return typeId;
}

PoisonerApp::PoisonerApp()
{
   NS_LOG_FUNCTION_NOARGS();
}

void
PoisonerApp::OnInterest(shared_ptr<const Interest> interest)
{
  //log that I received the interest
  App::OnInterest(interest);
  NS_LOG_FUNCTION(this << interest);

  //if I am not active, exit
  if(!m_active)
    return;

   //do data creation
   Name dataName(interest->getName());
   //no need for "/" because append already adds the slash...
   name::Component suffixName("evil");
   dataName.append(suffixName);
   auto data = make_shared<Data>();
   data->setName(dataName);
   data->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));
   data->setContent(make_shared<::ndn::Buffer>(m_payloadSize));
//    Signature sig;
//    SignatureInfo sigInfo(static_cast<::ndn::tlv::SignatureTypeValue>(255));

//    if(m_keyLocator.size() > 0)
//    {
//      sigInfo.setKeyLocator(m_keyLocator);
//    }
//    sig.setInfo(sigInfo);
//    sig.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));
//    data->setSignature(sig);
  
  //log that I am sending the data
  NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with Data:" << data->getName());

  data->wireEncode(); 

  //transmit the data
  m_transmittedDatas(data, this, m_face);
  m_appLink->onReceiveData(*data);
}

//inherited from application (overriding them to do more than just app stuff)
void
PoisonerApp::StartApplication()
{
   //original logs, calls parent start, and then call FibHelper to add route to self
   NS_LOG_FUNCTION_NOARGS();
   App::StartApplication();
   FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
}

void
PoisonerApp::StopApplication()
{
    NS_LOG_FUNCTION_NOARGS();
    App::StopApplication();
}

} // namespace ndn
} // namespace ns3