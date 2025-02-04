// Include necessary ROS2 and system headers
#include <arduinobot_controller/arduinobot_interface.hpp>
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <pluginlib/class_list_macros.hpp>

namespace arduinobot_controller {

/**
 * Helper function to format position commands for Arduino serial communication
 * Ensures all position values are sent as three-digit numbers
 * param value: The integer value to format
 * return: String of leading zeros needed ("00", "0", or "")
 */
std::string compensateZeros(int value) {
    std::string compensate_zeros = "";
    if(value < 10) {
        compensate_zeros = "00";  // For single digit numbers (e.g., 5 -> "005")
    } else if(value < 100) {
        compensate_zeros = "0";   // For double digit numbers (e.g., 45 -> "045")
    } else {
        compensate_zeros = "";    // For three digit numbers (e.g., 180 -> "180")
    }
    return compensate_zeros;
}

// Constructor - Currently empty as initialization happens in on_init
ArduinobotInterface::ArduinobotInterface() {}

/**
 * Destructor - Ensures clean shutdown by closing Arduino serial connection
 * Logs fatal error if connection closure fails
 */
ArduinobotInterface::~ArduinobotInterface() {
    if(arduino_.IsOpen()) {
        try {
            arduino_.Close();
        } catch(...) {
            RCLCPP_FATAL_STREAM(rclcpp::get_logger("ArduinobotInterface"),
                               "Something went wrong while closing the connection with port " << port_);
        }
    }
}

/**
 * Initialization callback - Called when the hardware interface is first initialized
 * Sets up serial port configuration and reserves memory for joint states
 */
CallbackReturn ArduinobotInterface::on_init(const hardware_interface::HardwareInfo & hardware_info) {
    // Initialize parent class first
    CallbackReturn result = hardware_interface::SystemInterface::on_init(hardware_info);
    if(result != CallbackReturn::SUCCESS) {
        return result;
    }

    // Get serial port from hardware parameters
    try {
        port_ = info_.hardware_parameters.at("port");
    } catch(const std::out_of_range &e) {
        RCLCPP_FATAL(rclcpp::get_logger("ArduinobotInterface"), "No Serial Port provided! Aborting...");
        return CallbackReturn::FAILURE;
    }

    // Reserve vectors for joint positions
    position_commands_.reserve(info_.joints.size());
    position_states_.reserve(info_.joints.size());
    prev_position_commands_.reserve(info_.joints.size());

    return CallbackReturn::SUCCESS;
}

/**
 * Exports the state interfaces for the robot joints
 * These interfaces allow ROS2 to read the current position of each joint
 */
std::vector<hardware_interface::StateInterface> ArduinobotInterface::export_state_interfaces() {
    std::vector<hardware_interface::StateInterface> state_interfaces;
    
    // Create position state interface for each joint
    for(size_t i = 0; i < info_.joints.size(); i++) {
        state_interfaces.emplace_back(hardware_interface::StateInterface(
            info_.joints[i].name, 
            hardware_interface::HW_IF_POSITION, 
            &position_states_[i]
        ));
    }
    return state_interfaces;
}

/**
 * Exports the command interfaces for the robot joints
 * These interfaces allow ROS2 to send position commands to each joint
 */
std::vector<hardware_interface::CommandInterface> ArduinobotInterface::export_command_interfaces() {
    std::vector<hardware_interface::CommandInterface> command_interfaces;
    
    // Create position command interface for each joint
    for(size_t i = 0; i < info_.joints.size(); i++) {
        command_interfaces.emplace_back(hardware_interface::CommandInterface(
            info_.joints[i].name, 
            hardware_interface::HW_IF_POSITION, 
            &position_commands_[i]
        ));
    }
    return command_interfaces;
}

/**
 * Activation callback - Called when the hardware interface is activated
 * Initializes joint positions and establishes Arduino serial connection
 */
CallbackReturn ArduinobotInterface::on_activate(const rclcpp_lifecycle::State & previous_state) {
    RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"), "Starting the robot hardware...");

    // Initialize joint positions to zero
    position_commands_ = {0.0, 0.0, 0.0, 0.0};
    prev_position_commands_ = {0.0, 0.0, 0.0, 0.0};
    position_states_ = {0.0, 0.0, 0.0, 0.0};

    // Setup Arduino serial connection
    try {
        arduino_.Open(port_);
        arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    } catch(...) {
        RCLCPP_FATAL_STREAM(rclcpp::get_logger("ArduinobotInterface"), 
                           "Something went wrong while interacting with the port " << port_);
        return CallbackReturn::FAILURE;
    }

    RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"), "Hardware started, ready to take commands");
    return CallbackReturn::SUCCESS;
}

/**
 * Deactivation callback - Called when the hardware interface is deactivated
 * Ensures clean shutdown of Arduino serial connection
 */
CallbackReturn ArduinobotInterface::on_deactivate(const rclcpp_lifecycle::State & previous_state) {
    RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"), "Stopping the robot hardware...");

    if(arduino_.IsOpen()) {
        try {
            arduino_.Close();
        } catch(...) {
            RCLCPP_FATAL_STREAM(rclcpp::get_logger("ArduinobotInterface"), "Hardware stopped");
            return CallbackReturn::FAILURE;
        }
    }

    RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"), "Hardware stopped");
    return CallbackReturn::SUCCESS;
}

/**
 * Read callback - Updates the current state of the robot
 * In this implementation, we assume the commanded position is achieved instantly
 */
hardware_interface::return_type ArduinobotInterface::read(
    const rclcpp::Time & time,
    const rclcpp::Duration & period) {
    
    position_states_ = position_commands_;
    return hardware_interface::return_type::OK;
}

/**
 * Write callback - Sends new position commands to the Arduino
 * Converts ROS radians to Arduino degrees and formats the message
 */
hardware_interface::return_type ArduinobotInterface::write(
    const rclcpp::Time & time,
    const rclcpp::Duration & period) {

    // Skip if no new commands
    if(position_commands_ == prev_position_commands_) {
        return hardware_interface::return_type::OK;
    }

    std::string msg;

    // Convert base joint position (radians to degrees, offset by 90°)
    int base = static_cast<int>((position_commands_.at(0) + (M_PI/2)) * 180) / M_PI;
    msg.append("b" + compensateZeros(base) + std::to_string(base) + ",");

    // Convert shoulder joint position (inverted, radians to degrees, offset by 90°)
    int shoulder = 180 - static_cast<int>(((position_commands_.at(1) + (M_PI/2)) * 180) / M_PI);
    msg.append("s" + compensateZeros(shoulder) + std::to_string(shoulder) + ",");

    // Convert elbow joint position (radians to degrees, offset by 90°)
    int elbow = static_cast<int>(((position_commands_.at(2) + (M_PI/2)) * 180) / M_PI);
    msg.append("e" + compensateZeros(elbow) + std::to_string(elbow) + ",");

    // Convert gripper position (radians to degrees, scaled and inverted)
    int gripper = static_cast<int>((-position_commands_.at(3) * 180) / (M_PI/2));
    msg.append("g" + compensateZeros(gripper) + std::to_string(gripper) + ",");

    // Send formatted message to Arduino
    try {
        arduino_.Write(msg);
    } catch(...) {
        RCLCPP_ERROR_STREAM(rclcpp::get_logger("ARduinobotInterface"), 
                           "Something went wrong while sending the message " << msg << 
                           " to the port " << port_);
        return hardware_interface::return_type::ERROR;
    }

    prev_position_commands_ = position_commands_;
    return hardware_interface::return_type::OK;
}

} // namespace arduinobot_controller

// Register this controller as a ROS2 hardware interface plugin
PLUGINLIB_EXPORT_CLASS(arduinobot_controller::ArduinobotInterface, hardware_interface::SystemInterface);