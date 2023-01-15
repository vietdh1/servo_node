#include <unistd.h>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include "dynamixel_sdk/dynamixel_sdk.h"
#include "dynamixel_sdk_custom_interfaces/msg/set_position.hpp"
//including message type



#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using std::placeholders::_1;
using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

int angle1 = 0;
int angle2 = 0;

float pi = 3.14159265359;
float L1 = 68;
float L2 = 38;
void inverseKinematics(float x, float y){
float angle11 ;      
float angle21 ;       
float rad_angle1;  
float rad_angle2; 
 
rad_angle2 = acos((pow(x,2)+ pow(y,2) - pow(L1,2) - pow(L2,2)) / (2*L1*L2));
rad_angle1= atan(y / x) - atan((L2*sin(rad_angle2)) / (L1+ L2*cos(rad_angle2)));
angle11= (rad_angle1*180)/pi;
angle21= (rad_angle2*180)/pi;
angle1 = 512 + 512 * (angle11/150);
angle2 = 512 + 512 * (angle21/150);
 }

class MinimalPublisher : public rclcpp::Node
{
  public:
    MinimalPublisher()
    : Node("minimal_publisher") 
    {
      
      subscription_ = this->create_subscription<std_msgs::msg::String>("servo_pos", 10, std::bind(&MinimalPublisher::topic_callback, this, _1));
      
      publisher_ = this->create_publisher<dynamixel_sdk_custom_interfaces::msg::SetPosition>("set_position",10);
      //sending data every 10 ms
      timer_ = this->create_wall_timer(
      10ms, std::bind(&MinimalPublisher::timer_callback, this));
    }

  private:
    
    void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
      {
        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
      //reading data from node
      std::string variable = msg->data.c_str();
      switch (variable[0]){
        case '0':
          inverseKinematics(105.9,0.00001);
          break;
        case '1':
          inverseKinematics(68,38);
          break;
        case '2':
          inverseKinematics(68,-38);
          break;
        case '3':
          inverseKinematics(74.95,74.95);
          break;
        case '4':
          inverseKinematics(74.95,-74.95);
          break;

      }

        //usleep(2000000);
      }
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    
    
    void timer_callback()
    {
 
      auto message = dynamixel_sdk_custom_interfaces::msg::SetPosition(); 
      //calculate the inverse kinematic
      
         
	
      //RCLCPP_INFO(this->get_logger(), "Publishing: '%d %d'", message.id,message.position);
      //writing the angle value on the 1st motor
      message.id = 1;
      message.position = angle1;      
      publisher_->publish(message);
      //writing the angle value on the 2nd motor
      message.id = 0;
      message.position = angle2;
      publisher_->publish(message);
      
    }
    rclcpp::TimerBase::SharedPtr timer_;
    //rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::Publisher<dynamixel_sdk_custom_interfaces::msg::SetPosition>::SharedPtr publisher_;

};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}
