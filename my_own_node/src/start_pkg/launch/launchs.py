from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    node = Node(
        package = "subscriber_node_pkg",
        executable="subscriber_node"
    )
    
    launch = LaunchDescription(
        [node]
    )
    return launch
