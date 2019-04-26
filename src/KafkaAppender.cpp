
#include "PortabilityImpl.hh"
#ifdef LOG4CPP_HAVE_IO_H
#include <io.h>
#endif
#ifdef LOG4CPP_HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <memory>
#include <stdio.h>
#include <time.h>
#include <log4cpp/KafkaAppender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FactoryParams.hh>

namespace log4cpp
{

// 事件回调
class ExampleEventCb : public RdKafka::EventCb
{
  public:
    void event_cb(RdKafka::Event &event)
    {
        std::cout << "ExampleEventCb event_cb" << std::endl;

        switch (event.type())
        {
        case RdKafka::Event::EVENT_ERROR:
            if (event.fatal())
            {
                std::cerr << "FATAL ";
                // run = false;

                // kafka出错，应该重启kafka连接，或者提示错误
            }
            std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " << event.str() << std::endl;
            break;

        case RdKafka::Event::EVENT_STATS:
            std::cerr << "\"STATS\": " << event.str() << std::endl;
            break;

        case RdKafka::Event::EVENT_LOG:
            fprintf(stderr, "LOG-%i-%s: %s\n",
                    event.severity(), event.fac().c_str(), event.str().c_str());
            break;

        default:
            std::cerr << "EVENT " << event.type() << " (" << RdKafka::err2str(event.err()) << "): " << event.str() << std::endl;
            break;
        }
    }
};

// 数据异步发送结果回调
void ExampleDeliveryReportCb::dr_cb(RdKafka::Message &message)
{
    std::cout << "ExampleDeliveryReportCb dr_cb" << std::endl;

    std::string status_name;
    switch (message.status())
    {
    case RdKafka::Message::MSG_STATUS_NOT_PERSISTED:
        status_name = "NotPersisted";
        break;
    case RdKafka::Message::MSG_STATUS_POSSIBLY_PERSISTED:
        status_name = "PossiblyPersisted";
        break;
    case RdKafka::Message::MSG_STATUS_PERSISTED:
        status_name = "Persisted";
        break;
    default:
        status_name = "Unknown?";
        break;
    }
    std::cout << "Message delivery for (" << message.len() << " bytes): " << status_name << ": " << message.errstr() << std::endl;
    if (message.key())
        std::cout << "Key: " << *(message.key()) << ";" << std::endl;
}

KafkaAppender::KafkaAppender(const std::string &name, const std::string &brokers, const std::string &topic, int timeout)
    : LayoutAppender(name), _brokers(brokers), _topic(topic), _timeout(timeout)
{

    std::cout << "kafka appender name : " << name << ", brokers : " << brokers << ",topic " << _topic << ",timeout " << _timeout << std::endl;

    // 连接kafka

    std::string errstr;
    int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;

    /*
   * Create configuration objects
   */
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    // 设置brokers
    conf->set("metadata.broker.list", brokers, errstr);

    // 设置事件回调
    // ExampleEventCb ex_event_cb;
    // conf->set("event_cb", &ex_event_cb, errstr);

    /* Set delivery report callback 异步数据发送结果回调 */
    ex_dr_cb = new ExampleDeliveryReportCb;
    conf->set("dr_cb", ex_dr_cb, errstr);

    // 设置topic
    conf->set("default_topic_conf", tconf, errstr);

    // 创建producer
    _producer = RdKafka::Producer::create(conf, errstr);

    if (!_producer)
    {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    // producer name
    // producer->name();
}
KafkaAppender::~KafkaAppender()
{
    // 析构，释放kafka
    // destroy();
}

bool KafkaAppender::reopen()
{
    // 重新打开kafka

    return true;
}

void KafkaAppender::close()
{
    // 关闭kafka连接

    while (_producer->outq_len() > 0)
    {
        std::cerr << "Waiting for " << _producer->outq_len() << std::endl;
        _producer->poll(1000);
    }

    delete _producer;
    _producer = nullptr;

    RdKafka::wait_destroyed(5000);

    delete ex_dr_cb;
    ex_dr_cb = nullptr;
}

void KafkaAppender::_append(const LoggingEvent &event)
{
    // std::cout << "kafka " << _getLayout().format(event);
    // fprintf(&std::cout, "kafka append");

    std::string message(_getLayout().format(event));

    std::cout << "kafka appender " << message << std::endl;
    // 写入kafka

    RdKafka::Headers *headers = RdKafka::Headers::create();
    headers->add("my header", "header value");
    headers->add("other header", "yes");

    RdKafka::ErrorCode resp =
        _producer->produce(_topic, _partition,
                           RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
                           /* Value */
                           const_cast<char *>(message.c_str()), message.size(),
                           /* Key */
                           NULL, 0,
                           /* Timestamp (defaults to now) */
                           0,
                           /* Message headers, if any */
                           headers,
                           /* Per-message opaque value passed to
                           * delivery report */
                           NULL);

    if (resp != RdKafka::ERR_NO_ERROR)
    {
        std::cerr << "% Produce failed: " << RdKafka::err2str(resp) << std::endl;
        delete headers; /* Headers are automatically deleted on produce()
                         * success. */
    }
    else
    {
        std::cerr << "% Produced message (" << message.size() << " bytes)" << std::endl;
    }

    _producer->poll(_timeout);
}

} // namespace log4cpp
