#include "main.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "mros2.hpp"
#include "mros2_msgs/std_msgs/msg/float32.hpp"

extern "C" void StartDefaultTask(void *argument)
{
	  mros2::init();
	  printf("mROS 2 initialization is completed");

	  mros2::Node node = mros2::Node::create_node("mros2_node");
	  mros2::Publisher pub = node.create_publisher<std_msgs::msg::Float32>("to_linux", 10);
	  osDelay(100);
	  printf("ready to pub/sub message\r\n---");

	  std_msgs::msg::Float32 msg;
	  auto publish_count = -0.5;
	  while (1)
	  {
	    msg.data = publish_count;
	    printf("publishing float msg!!");
	    pub.publish(msg);

	    if (0.0 >= msg.data)
	    	printf("msg <= 0.0");
	    else if (0.0 < msg.data && msg.data < 0.5)
	    	printf("0.0 < msg < 0.5");
	    else if (0.5 < msg.data && msg.data < 1.0)
	    	printf("0.5 < msg < 1.0");
	    else
	    	printf("msg >= 1.0");

	    publish_count = publish_count + 0.1;
	    osDelay(10);
	  }

	  mros2::spin();
  /* USER CODE END 5 */
}