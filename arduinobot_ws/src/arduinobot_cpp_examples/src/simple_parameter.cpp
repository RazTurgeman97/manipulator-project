#include <rclcpp/rclcpp.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>
#include <string>
#include <vector>
#include <memory>

using std::placeholders::_1;

class SimpleParameter : public rclcpp::Node
{
public:
    SimpleParameter() : Node("simple_parameter") {
        declare_parameter<int>("simple_int_param", 28); // Note that declare_parameter<int> can also be written as declare_parameter("simple_int_param", 28)
        declare_parameter<double>("simple_double_param", 3.14); // Note that declare_parameter<double> can also be written as declare_parameter("simple_double_param", 3.14)
        declare_parameter<std::string>("simple_string_param", "Hello, ROS2!"); // Note that declare_parameter<std::string> can also be written as declare_parameter("simple_string_param", "Hello, ROS2!")

        param_callback_handle_ = add_on_set_parameters_callback(std::bind(&SimpleParameter::paramChangeCallback, this, _1)); // std::bind is used to bind the paramChangeCallback function to the add_on_set_parameters_callback function
    }

private:
    OnSetParametersCallbackHandle::SharedPtr param_callback_handle_;

    rcl_interfaces::msg::SetParametersResult paramChangeCallback(const std::vector<rclcpp::Parameter> &parameters){
        rcl_interfaces::msg::SetParametersResult result;
        for (const auto &param : parameters) {
            if (param.get_name() == "simple_int_param" && param.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER) {
                RCLCPP_INFO_STREAM(get_logger(), "Parameter simple_int_param changed! New value is: " << param.as_int());
                result.successful = true;
            }
            else if (param.get_name() == "simple_double_param" && param.get_type() == rclcpp::ParameterType::PARAMETER_DOUBLE) {
                RCLCPP_INFO_STREAM(get_logger(), "Parameter simple_double_param changed! New value is: " << param.as_double());
                result.successful = true;
            }
            else if (param.get_name() == "simple_string_param" && param.get_type() == rclcpp::ParameterType::PARAMETER_STRING) {
                RCLCPP_INFO_STREAM(get_logger(), "Parameter simple_string_param changed! New value is: " << param.as_string());
                result.successful = true;
            }
        }

        return result;
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SimpleParameter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}   