#include <rclcpp/rclcpp.hpp> // ROS2 C++ API
#include <std_msgs/msg/string.hpp> // ROS2 std_msgs

#include <chrono> // C++ chrono library

using namespace std::chrono_literals; // Using namespace for the literals


class SimplePublisher : public rclcpp::Node{ // SimplePublisher class inherits from rclcpp::Node

public: // Public members

    SimplePublisher() : Node("simple_publisher"), counter_(0){ // Constructor for SimplePublisher class

        pub_ = create_publisher<std_msgs::msg::String>("chatter", 10); // Create a publisher to the topic "chatter"
        timer_ = create_wall_timer(1s, std::bind(&SimplePublisher::timerCallback, this)); // Create a timer to publish messages at 1 Hz

        RCLCPP_INFO(get_logger(), "Publishing at 1 Hz"); // Print a message to the console
    }

    void timerCallback(){
        auto message = std_msgs::msg::String(); // Create a message
        message.data = "Hello ROS2 - counter: " + std::to_string(counter_++); // Set the message data
        pub_->publish(message); // Publish the message
    }

private:

    unsigned int counter_; // Counter for the number of messages published
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_; // Publisher to the topic "chatter"
    rclcpp::TimerBase::SharedPtr timer_; // Timer to publish messages at 1 Hz

};


int main(int argc, char* argv[]){

    rclcpp::init(argc, argv); // Initialize ROS2
    auto node = std::make_shared<SimplePublisher>(); // Create a shared pointer to a SimplePublisher object
    rclcpp::spin(node); // Spin the node
    rclcpp::shutdown(); // Shutdown ROS2
    return 0;
    
}