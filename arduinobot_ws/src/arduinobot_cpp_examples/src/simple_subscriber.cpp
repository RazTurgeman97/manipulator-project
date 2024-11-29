#include <rclcpp/rclcpp.hpp> // ROS2 C++ API
#include <std_msgs/msg/string.hpp> // ROS2 std_msgs

using std::placeholders::_1; // Using namespace for the placeholders

class SimpleSubscriber : public rclcpp::Node{ // SimpleSubscriber class inherits from rclcpp::Node

public:

    SimpleSubscriber() : Node("simple_subscriber"){ // Constructor for SimpleSubscriber class

        sub_ = create_subscription<std_msgs::msg::String>("chatter", 10, std::bind(&SimpleSubscriber::msgCallback, this, _1)); // Create a subscription to the topic "chatter"
    }

    void msgCallback(const std_msgs::msg::String &msg) const{ // Callback function for the subscription

        RCLCPP_INFO_STREAM(get_logger(), "I heared: " << msg.data.c_str()); // Print the message to the console
    }

private:

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_; // Subscription to the topic
};

int main(int argc, char * argv[]){

    rclcpp::init(argc, argv); // Initialize ROS2    
    auto node = std::make_shared<SimpleSubscriber>(); // Create a shared pointer to a SimpleSubscriber object
    rclcpp::spin(node); // Spin the node
    rclcpp::shutdown(); // Shutdown ROS2
    return 0;
}