import rclpy
from rclpy.node import Node
from rcl_interfaces.msg import SetParametersResult
from rclpy.parameter import Parameter

class SimpleParameter(Node):
    def __init__(self):
        super().__init__('simple_parameter')
        self.declare_parameter('simple_int_param', 28)
        self.declare_parameter('simple_double_param', 3.14)
        self.declare_parameter('simple_string_param', 'Hello, ROS2!')
        
        self.add_on_set_parameters_callback(self.paramChangeCallback)

    def paramChangeCallback(self, parameters):
        # SetParametersResult is a message that is used to return the result of a set_parameters request.
        result = SetParametersResult()
        
        for param in parameters:
            if param.name == 'simple_int_param' and type(param.value) is int:
                # Note that 'type(param.value) is int' can also be written as param.type_ == Parameter.Type.INTEGER
                self.get_logger().info("Parameter " + f'{param.name}' + " changed! New value is: " + f'{param.value}')
                result.successful = True
            elif param.name == 'simple_double_param' and type(param.value) is float:
                self.get_logger().info("Parameter " + f'{param.name}' + " changed! New value is: " + f'{param.value}')
                result.successful = True
            elif param.name == 'simple_string_param' and type(param.value) is str:
                self.get_logger().info("Parameter " + f'{param.name}' + " changed! New value is: " + f'{param.value}')
                # Note that '+ f'{param.value}' can also be written as '%s' % param.value
                result.successful = True
                
        return result

def main(args=None):
    rclpy.init(args=args)
    node = SimpleParameter()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()