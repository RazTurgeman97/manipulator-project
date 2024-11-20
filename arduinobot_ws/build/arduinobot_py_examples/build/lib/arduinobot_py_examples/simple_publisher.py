import rclpy # ROS Client Library for python
from rclpy.node import Node # Node module from the rclpy library
from std_msgs.msg import String # The String nessage is defined on this library

class SimplePublisher(Node): # A class called "SimplePublisher" which inherit from the ndoe class of the rclpy library
    def __init__(self): # The class constructor - the function that is automatically executed when a new object of the class is created.
        super().__init__("simple_publisher") # calling the constrictor of the node class with the name "simple_publisher"
        self.pub_ = self.create_publisher(String, "chatter", 10) # Creating publisher object with the pubvlisher function from the node class
        # We need to indicate the message type so the type of the interface that we need to use for the message exchange with the topic.
        # In practice, we need to specify which type of messages the publisher object will send through the topic.
        # create_publisher(message type, topic name, queue size which works as a buffer)
        self.counter_ = 0
        self.frequency_ = 1.0
        self.get_logger().info("Publishing at %d Hz" % self.frequency_) # Print an informative message in the terminal.
        self.timer_ = self.create_timer(self.frequency_, self.timerCallback) # create timer function takes as input the frequency at which we want to
        # execute a certain function and it also takes the name of the function, timerCallback, to be executed at regular time intervals.
        
    def timerCallback(self): # defining timerCallback function which is a member of the SimplePublisher class. Its goal is to publish a new message within the chapter topic.
        
        msg = String()
        msg.data = "Hello ROS2 - counter: %d" % self.counter_
        self.pub_.publish(msg)
        self.counter_ += 1
        
def main():
    rclpy.init() # initialize ROS2
    simple_publsher = SimplePublisher()
    rclpy.spin(simple_publsher) # keeping the node running
    simple_publsher.destroy_node() # ensure the SimplePublisher node is properly destroyed when termianting.
    rclpy.shutdown() # shutdown ROS2
        
if __name__ == '__main__':
    main()