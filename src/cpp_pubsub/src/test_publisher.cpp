#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <stdio.h>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

class TestPublisher : public rclcpp::Node
{
  public:
    TestPublisher()
    : Node("test_publisher_node")
    {
      publisher_2 = this->create_publisher<std_msgs::msg::String>("servo_pos", 10);
      timer_ = this->create_wall_timer(
      2000ms, std::bind(&TestPublisher::timer_callback, this));
    }

  private:
    void timer_callback()
    {
      int prediction;
      prediction ++;
      if(prediction>4){
        prediction = 0;
      }
      auto message = std_msgs::msg::String();
      message.data = std::to_string(prediction);
      RCLCPP_INFO(this->get_logger(), "Prediction: '%s'", message.data.c_str());
      publisher_2->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_2;
    

};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TestPublisher>());
  rclcpp::shutdown();
  return 0;
}
