import os
from pathlib import Path
from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, SetEnvironmentVariable
from launch.substitutions import Command, LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource

from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    
    arduinobot_description_path = get_package_share_directory('arduinobot_description')
    
    model_arg = DeclareLaunchArgument(name='model',
                                      default_value=os.path.join(arduinobot_description_path, 'urdf', 'arduinobot.urdf.xacro'),
                                      description='Absolute path to robot urdf file'
    )
    
    gazebo_resource_path = SetEnvironmentVariable(name='GZ_SIM_RESOURCE_PATH',
                                                  value=[
                                                    str(Path(arduinobot_description_path).parent.resolve())
                                                ]
    )
    
    ros_distro = os.environ["ROS_DISTRO"]
    is_ignition = "True" if ros_distro == "humble" else "False"
    physics_engine = "" if ros_distro == "humble" else "--physics-engine gz-physics-bullet-featherstone-plugin"
    
    robot_description = ParameterValue(Command(['xacro ', LaunchConfiguration('model')]),
                                       value_type=str)
    
    robot_state_publisher = Node(package='robot_state_publisher', 
                                 executable='robot_state_publisher', 
                                 name='robot_state_publisher', 
                                 parameters=[{'robot_description': robot_description,
                                              'use_sim_time': True}]
    )
    
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            os.path.join(
                get_package_share_directory("ros_gz_sim"),
                'launch'
            ),  '/gz_sim.launch.py']
        ),
        launch_arguments=[
            ('gz_args', [' -v 4 -r empty.sdf ', physics_engine]),
            ('world', os.path.join(arduinobot_description_path, 'worlds', 'empty.sdf'))
        ]
    )
    
    gz_spawn_entity = Node(package='ros_gz_sim',
                           executable='create',
                           arguments=['-topic', 'robot_description',
                                      '-name', 'arduinobot'],
                           output='screen'
    )
    
    gz_ros2_bridge = Node(package='ros_gz_bridge',
                           executable='parameter_bridge',
                           arguments=['/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock',
                                      '/cmd_vel@geometry_msgs/msg/Twist[gz.msgs.Twist',
                                      '/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry',
                                      "/image_raw@sensor_msgs/msg/Image[gz.msgs.Image",
                                      "/camera_info@sensor_msgs/msg/CameraInfo[gz.msgs.CameraInfo",
                            ]
    )
    
    return LaunchDescription([
        model_arg,
        gazebo_resource_path,
        robot_state_publisher,
        gazebo,
        gz_spawn_entity,
        gz_ros2_bridge
    ])
