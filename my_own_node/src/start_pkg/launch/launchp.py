from launch import LaunchDescription
from launch_ros.actions import Node
import os

def generate_launch_description():
    
    launch_dir = os.path.dirname(__file__)
    workspace_dir = os.path.abspath(os.path.join(
        launch_dir,  
        '..',     
        '..',
        '..',
        '..',
        '..'
    ))
    
    
    params_file = os.path.abspath(os.path.join(
        workspace_dir,
        'src',
        'publisher_node_pkg',  
        'config',  
        'can.yaml'  
    ))
    node = Node(
        package="publisher_node_pkg",
        executable="publisher_node",
        parameters=[params_file]
    )
    
    launch = LaunchDescription(
        [node]
    )
    return launch