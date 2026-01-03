from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

#这个是启动两个节点的脚本，不建议使用，建议在一个终端里面启动发布者，另一个启动接受者
def generate_launch_description():
    
    
    #这个是静态的路径（每次改参数要重新编译）
    """   
    params_file = os.path.abspath(os.path.join(
    get_package_share_directory("publisher_node_pkg"),
    'config',
    'can.yaml'
))
    """
    
    
    #这个是动态路径（一个及其神经的相对路径，单数好用）
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
    
    #创建两个节点（（依旧依葫芦画瓢））
    #实际上改一下下面的名字就可以通用了
    node_1 = Node(
        package = "publisher_node_pkg",
        executable="publisher_node",
        parameters=[params_file]
    )
    node_2 = Node(
        package = "subscriber_node_pkg",
        executable="subscriber_node"
    )
    
    launch_description = LaunchDescription(
        [node_1, node_2]
    )
    return launch_description
