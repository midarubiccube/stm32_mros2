
#include "mros2_msgs/std_msgs/msg/float32.hpp"
#include "mros2_msgs/geometry_msgs/msg/twist.hpp"


template mros2::Publisher mros2::Node::create_publisher<std_msgs::msg::Float32>(std::string topic_name, int qos);
template void mros2::Publisher::publish(std_msgs::msg::Float32 &msg);

template mros2::Subscriber mros2::Node::create_subscription(std::string topic_name, int qos, void (*fp)(geometry_msgs::msg::Twist*));
template void mros2::Subscriber::callback_handler<geometry_msgs::msg::Twist>(void *callee, const rtps::ReaderCacheChange &cacheChange);