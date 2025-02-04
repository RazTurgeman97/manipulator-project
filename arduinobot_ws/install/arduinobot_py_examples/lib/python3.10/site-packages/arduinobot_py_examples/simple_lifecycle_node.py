import time
import rclpy
import rclpy.executors
from rclpy.lifecycle import Node, State, TransitionCallbackReturn
from std_msgs.msg import String


class SimpleLifecycleNode(Node):
    def __init__(self, node_name, **kwargs):
        super().__init__(node_name, **kwargs)
        
    # Lifecycle created and in Uncofigured state. Options: configure or shutdown.
    
    # Configure:
        
    def on_configure(self, state: State) -> TransitionCallbackReturn:
        self.sub_ = self.create_subscribtion(String, "chatter", self.msgCallback, 10)
        self.get_logger().info("Lifecycle Node on_configure() called.")
        return TransitionCallbackReturn.SUCCESS
    # After configuration it changes the state to an Inactive state. Options: Cleanup (back to unconfigured), shutdown or Activate.
    
    # Shutdown:
    
    def on_shutdown(self, state: State) -> TransitionCallbackReturn:
        self.destroy_subscription(self.sub_)
        self.get_logger().info("Lifecycle Node on_shutdown() called.")
        return TransitionCallbackReturn.SUCCESS
    # After shutdown changes the state to Finalized state and it can only be destroyed.
    
    # Cleanup:
    
    def on_cleanup(self, state: State) -> TransitionCallbackReturn:
        self.destroy_subscription(self.sub_)
        self.get_logger().info("Lifecycle Node on_cleanup() called.")
        return TransitionCallbackReturn.SUCCESS
    # Back to Inactive state.
    
    # Activate:
    
    def on_activate(self, state: State) -> TransitionCallbackReturn:
        self.get_logger().info("Lifecycle Node on_activate() called.")
        time.sleep(2)
        return super().on_activate(state)
    # After activate changes the state to Active state. Options: deacticate or shutdown.
    
    def on_deactivate(self, state: State) -> TransitionCallbackReturn:
        self.get_logger().info("Lifecycle Node on_deactivate() called.")
        return super().on_deactivate(state)
    
    def msgCallback(self, msg):
        current_state = self._state_machine.current_state
        if current_state[1] == "active":
            self.get_logger().info("I heard: %s" %msg.data)
            
            
def main():
    rclpy.init()
    executor = rclpy.executors.SingleThreadedExecutor()
    lifecycle_node = SimpleLifecycleNode("simple_lifecycle_node")
    executor.add_node(lifecycle_node)
    try:
        executor.spin()
    except (KeyboardInterrupt, rclpy.executors.ExternalShutdownException):
        lifecycle_node.destroy_node()
        
if __name__ == "__main__":
    main()