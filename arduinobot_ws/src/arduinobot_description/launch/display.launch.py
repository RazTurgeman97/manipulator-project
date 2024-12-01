from launch import LaunchDescription # LaunchDescription is used to describe the launch file
from launch_ros.actions import Node # Node is used to create a node
from launch_ros.parameter_descriptions import ParameterValue # ParameterValue is used to create a parameter value
from launch.actions import DeclareLaunchArgument # DeclareLaunchArgument is used to declare a launch argument
from launch.substitutions import Command, LaunchConfiguration # Command is used to create a command, LaunchConfiguration is used to get the value of a launch argument
import os
from ament_index_python import get_package_share_directory

# ros2 launch arduinobot_description display.launch.py

def generate_launch_description():
    model_arg = DeclareLaunchArgument(name='model',
                                      default_value=os.path.join(get_package_share_directory('arduinobot_description'), 'urdf', 'arduinobot.urdf.xacro'),
                                      description='Absolute path to robot urdf file'
    )
    
    robot_description = ParameterValue(Command(['xacro ', LaunchConfiguration('model')]))
    
    robot_state_publisher = Node(package='robot_state_publisher', 
                                 executable='robot_state_publisher', 
                                 name='robot_state_publisher', 
                                 parameters=[{'robot_description': robot_description}]
    )
    
    joint_state_publisher_gui = Node(package='joint_state_publisher_gui', 
                                    executable='joint_state_publisher_gui', 
                                    name='joint_state_publisher_gui'
    )
    
    rviz_node = Node(package='rviz2', 
                     executable='rviz2', 
                     name='rviz2', 
                     output='screen', 
                     arguments=['-d', os.path.join(get_package_share_directory('arduinobot_description'), 'rviz', 'display.rviz')]
    )
    
    return LaunchDescription([
        model_arg,
        robot_state_publisher,
        joint_state_publisher_gui,
        rviz_node
    ])