/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "mros2.hpp"
#include "CANFD.hpp"
#include "usart.h"
#include "mros2_msgs/std_msgs/msg/float32.hpp"
#include "mros2_msgs/geometry_msgs/msg/twist.hpp"

CANFD* canfd;

void userCallback(geometry_msgs::msg::Twist* msg)
{
  printf("mROS 2 initialization is completed");
  CANFD_Frame frame;
  memcpy(frame.data, (uint8_t*)&msg->angular.x, 64);
  frame.size = 64;
  canfd->tx(frame);
  uint8_t msgs[] = "hello world!\n\r";
  HAL_UART_Transmit(&huart3, msgs, strlen((char*)msgs), 100);
}

extern "C" void MainTask(void *argument)
{
    canfd = new CANFD(&hfdcan1);
	  canfd->init();

    CANFD_Frame test;
	  test.id=10;
	  test.size = 64;
  	memset(test.data, 0, 64);
	  canfd->tx(test);

	  mros2::init();
	  printf("mROS 2 initialization is completed");

	  mros2::Node node = mros2::Node::create_node("mros2_node");
	  mros2::Publisher pub = node.create_publisher<std_msgs::msg::Float32>("to_linux", 10);
    mros2::Subscriber sub = node.create_subscription<geometry_msgs::msg::Twist>("cmd_vel", 10, userCallback);

	  osDelay(100);
	  printf("ready to pub/sub message\r\n---");

	  std_msgs::msg::Float32 msg;
	  auto publish_count = -0.5;
	  while (1)
	  {
	    msg.data = publish_count;
	    pub.publish(msg);
	    publish_count = publish_count + 0.1;
	    osDelay(100);
	  }

	  mros2::spin();
  /* USER CODE END 5 */
}