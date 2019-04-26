
#ifndef _LOG4CPP_KAFKAAPPENDER_HH
#define _LOG4CPP_KAFKAAPPENDER_HH

#include <log4cpp/Portability.hh>
#include <log4cpp/LayoutAppender.hh>
#include <string>
#include <stdarg.h>
#include <librdkafka/rdkafkacpp.h>
#include <librdkafka/rdkafka.h>

namespace log4cpp
{

// 数据异步发送结果回调
class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb
{
public:
  void dr_cb(RdKafka::Message &message);
};

class LOG4CPP_EXPORT KafkaAppender : public LayoutAppender
{
public:
  KafkaAppender(const std::string &name, const std::string &brokers, const std::string &topic, int timeout);
  virtual ~KafkaAppender();

  virtual bool reopen();
  virtual void close();

protected:
  virtual void _append(const LoggingEvent &event);

  const std::string _brokers;
  const std::string _topic;
  const int _timeout;

  RdKafka::Producer *_producer = nullptr;

  int32_t _partition = RdKafka::Topic::PARTITION_UA;

  ExampleDeliveryReportCb *ex_dr_cb = nullptr;
};
} // namespace log4cpp

#endif