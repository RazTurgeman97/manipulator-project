import rclpy
from rclpy.node import Node
from arduinobot_msgs.srv import AddTwoInts


class SimpleServiceServer(Node):
    def __init__(self):
        super().__init__("simple_service_server")
        
        self.service_ = self.create_service(AddTwoInts, "add_two_ints", self.serviceCallback)
        self.get_logger().info("Service add_two_onts Ready.")
        
    def serviceCallback(self, req, res):
        self.get_logger().info("New Message Received a: %d, b: %d" % (req.a, req.b))
        res.sum = req.a + req.b
        self.get_logger().info("Returning sum: %d" %res.sum)
        return res
    
def main(args=None):
    rclpy.init(args=args)
    node = SimpleServiceServer()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()
    
if __name__ == '__main__':
    main()

        
