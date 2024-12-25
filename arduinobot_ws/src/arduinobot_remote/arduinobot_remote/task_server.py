import rclpy
from rclpy.node import Node
from rclpy.action import ActionServer
from arduinobot_msgs.action import ArduinobotTask
import time
from moveit.planning import MoveItPy
from moveit.core.robot_state import RobotState

### moveit API does not avalible for ROS2 Humble on python. only on C++

class TaskServer(Node):
    def __init__(self):
        super().__init__("task_server")
        self.action_server = ActionServer(self, ArduinobotTask, "task_server", self.goalCallback)
        self.get_logger().info("Starting the server")
        
    def goalCallback(self, goal_handle):
        self.get_logger().info("Received goal request with order %d" %goal_handle.request.order)
        feedback_msg = ArduinobotTask.Feedback()
        feedback_msg.partial_sequence = [0, 1]
        
        for i in range(1, goal_handle.request.order):
            feedback_msg.partial_sequence.append(feedback_msg.partial_sequence[i] + feedback_msg.partial_sequence[i-1])
            self.get_logger().info("Feedback: {0}".format(feedback_msg.partial_sequence))
            goal_handle.publish_feedback(feedback_msg)
            time.sleep(1)
            
        goal_handle.succeed()
        result = ArduinobotTask.Result()
        result.sequence = feedback_msg.partial_sequence
        return result

def main():
    rclpy.init()
    node = TaskServer()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()