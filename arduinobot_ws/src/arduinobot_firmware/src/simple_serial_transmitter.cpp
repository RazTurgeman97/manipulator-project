#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <libserial/SerialPort.h>

using std::placeholders::_1;

class SimpleSerialTransmitter : public rclcpp::Node{ 

public:

    SimpleSerialTransmitter() : Node("simple_serial_transmitter"){

        declare_parameter<std::string>("port", "/dev/ttyUSB0");
        std::string port_ = get_parameter("port").as_string();

        sub_ = create_subscription<std_msgs::msg::String>("serial_transmitter", 10, std::bind(&SimpleSerialTransmitter::msgCallback, this, _1)); // Create a subscription to the topic "chatter"
    
        arduino_.Open(port_);
        arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_1152000);
    }

    ~SimpleSerialTransmitter(){

        arduino_.Close();
    }

    void msgCallback(const std_msgs::msg::String &msg){

        RCLCPP_INFO_STREAM(get_logger(), "New message received. Pubishing on serial port: " << msg.data);
        arduino_.Write(msg.data);
    }

private:

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_; // Subscription to the topic
    LibSerial::SerialPort arduino_;
};

int main(int argc, char * argv[]){

    rclcpp::init(argc, argv); // Initialize ROS2    
    auto node = std::make_shared<SimpleSerialTransmitter>(); // Create a shared pointer to a SimpleSerialTransmitter object
    rclcpp::spin(node); // Spin the node
    rclcpp::shutdown(); // Shutdown ROS2
    return 0;
}