from launch import LaunchDescription # LaunchDescription is used to describe the launch file
from launch_ros.actions import Node # Node is used to create a node
from launch_ros.parameter_descriptions import ParameterValue # ParameterValue is used to create a parameter value
from launch.actions import DeclareLaunchArgument # DeclareLaunchArgument is used to declare a launch argument
from launch.substitutions import Command, LaunchConfiguration # Command is used to create a command, LaunchConfiguration is used to get the value of a launch argument
import os
from ament_index_python import get_package_share_directory

def generate_launch_description():
    # Declare arguments
    model_arg = DeclareLaunchArgument(name='model',
                                      default_value=os.path.join(get_package_share_directory('arduinobot_description'),
                                                                                            'urdf',
                                                                                            'manipulator.urdf.xacro'),
                                      description='Absolute path to robot urdf file'
    )
    
    robot_description = ParameterValue(Command(['xacro ', LaunchConfiguration('model')]))

    # Set paths
    pkg_path = get_package_share_directory('arduinobot_description')
    urdf_path = os.path.join(pkg_path, 'urdf', 'manipulator.urdf.xacro')
    rviz_config_path = os.path.join(pkg_path, 'rviz', 'display.rviz')

    # Create nodes
    joint_state_publisher_gui = Node(
        package='joint_state_publisher_gui',
        executable='joint_state_publisher_gui',
        name='joint_state_publisher_gui'
    )

    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        parameters=[{'robot_description': robot_description}]
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_config_path]
    )

    # Create and return launch description
    return LaunchDescription([
        model_arg,
        joint_state_publisher_gui,
        robot_state_publisher,
        rviz_node
    ]) 