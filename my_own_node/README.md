this program include the most of situation of ROS2 using including packages , transmission , DIY message , launching  ,  config transition

here are totol instruction , details would be exhibited int specific files


阅读顺序：先看这个，再看publisher_pkg(要关注一下config文件夹), subscriber_pkg ,msg_pkg ,start_pkg


FIRST OF ALL :
ROS actually is a frame work that for info-transition , so it is a code frame work(Just copy and paste can reach the most situation)

以下所有实例都在源代码中详细讲解，所有代码片段需要将项目名称（也许是文件名）改为自己的文件名才可以使用
以下代码仅供示范


package: 
ROS can be written in a single file(.cpp) , but you should write a valid CML and .xml , which is really complex . thus we can use ROS package to pack our code , and it can autonomously arrange your code , but you should make some addition to its file in order that your packages are availible .


creating packages (bash/terminal): 
ros2 pkg create demo_cpp_pkg --build-type ament_cmake --license Apache-2.0
ros2 pkg create表示创建功能包，demo_cpp_pkg为包的名称，ament_cmake为构建类型，--license Apache-2.0表明了包的开源许可协议。构建之后你应该看到以下的文件结构
src
│
demo_cpp_pkg
    │
    ├─ include
    ├─ src
    ├─ CMakeLists.txt
    ├─ LICENSE
    └─ package.xml

src下即为编写源代码的地方
同时，我们可以看到在创建功能包时自动生成了CMakeLists.txt且已经写有了一些东西,我们只需要进行一些小改动。理论上你会看到以下结构，对照上下文，把新增的代码插入其中

注：这里的修改是基于项目名称“ros2_node” ， 使用时将所有的“ros2_node” 换成你自己的项目名称


cmake_minimum_required(VERSION 3.8)
……
find_package(ament_cmake REQUIRED)

find_package(rclcpp REQUIRED)    #这里是找到rclcpp库，ROS的核心依赖

add_executable(ros2_node src/ros2_node.cpp) #这里使用你自己的源代码文件路径
ament_target_dependencies(ros2_node rclcpp)
install(TARGETS
    ros2_node
    DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
……
ament_package()


下来还要在 packages.xml里插入对rclcpp的依赖声明


 <?xml version="1.0"?>
 ……
  <license>Apache-2.0</license>
  
  <depend>rclcpp</depend>
  
  <buildtool_depend>ament_cmake</buildtool_depend>
  ……
  </package>


然后构建功能包： colcon build
或者 colcon build --packages-select 包名

运行：

source install/setup.bash
ros2 run 包名 程序名



文件结构：
ROS 使用时会有发布者（Publisher）接受者（subscriber）一般分别进行建包
发布者与接受者通过一个特定的topic进行通讯 ， 消息类型可以通过自定义消息接口实现
当然，如果只是传输一些简单的数据，可以直接用std_msg包实现，略




自定义消息接口：

建包：ros2 pkg create --build-type ament_cmake my_msgs_pkg --dependencies rosidl_default_generators

在功能包目录my_msgs下创建msg文件夹，在其中新建 Mymessage.msg，注意这个文件的命名只能以大写字母开头且仅由大小写字母和数字构成

比如：
string name
int8 number

这个文件为自定义消息，每一行都由消息的数据类型和变量名组成。接下来更改CMakeLists.txt文件来进行注册。插入如下代码

rosidl_generate_interfaces(${PROJECT_NAME}
"msg/Mymessage.msg"
)

修改package.xml

<member_of_group>rosidl_interface_packages</member_of_group>

接下来构建功能包，完成后可用下列代码验证构建是否成功

colcon build --packages-select my_msgs_pkg
source install/setup.bash
ros2 interface show my_msgs_pkg/msg/Mymessage

#你将看到如下结果
string name
int8 number





启动脚本：


在上面的操作中，你会发现编译、启动若干节点会需要手动输入不少代码，那能不能简化一下，让他们一键执行呢？launch就是ROS2中用于管理节点和进程的工具，下面简单演示通过它快速启动多个节点的方法

ROS2的launch文件有三种格式，python、xml、yaml。其中ROS2官方推荐的时python方式编写launch文件
ros2 pkg create robot_startup_pkg --build-type ament_cmake

在功能包内新建launch文件夹，新建文件launch.py，并写入以下程序

from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    """launch内容描述函数，由ros2 launch 扫描调用"""
    robot_node_1 = Node(
        package="publisher_node_pkg", '''这里是包名'''
        executable="publisher_node"    '''这里是程序名'''
    )
    robot_node_2 = Node(
        package="subscriber_node_pkg",  '''这里是包名'''
        executable="subscriber_node"     '''这里是程序名'''
    )
    
    # 创建LaunchDescription对象launch_description,用于描述launch文件
    launch_description = LaunchDescription(
        [robot_node_1, robot_node_2])
    # 返回让ROS2根据launch描述执行节点
    return launch_description


接下来需要把文件拷贝入install下，在CMakeLists里添加

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

接下来colcon build构建包，最后运行

source install/setup.bash
ros2 launch robot_startup_pkg launch.py



参数传递：

在前面的程序中，我们会发现一旦我们需要修改程序内部的内容，就需要重新编译，这样显然过于麻烦。能不能把一些常会更改的设置、参数等放入一个文件，直接让程序动态读取参数而不用重新构建呢？这就要用到参数传递了。
参数是节点的一个配置值，你可以认为参数是一个节点的设置。它用于在外部文件中更改节点内的各种信息。以下是参数传递的常用函数
declare_parameter            声明和初始化一个参数
describe_parameter(name)  通过参数名字获取参数的描述
get_parameter                通过参数名字获取一个参数
set_parameter                设置参数的值



首先在publisher_node_pkg文件夹下新建config文件夹用于存放参数文件，随后新建time.yaml（比如），输入参数

simple_publisher:
  ros__parameters:
    during_time: 1000

由于我们新建了config用于存储参数文件，为了让系统找到他的位置，需要用install把他复制到install目录下。因此在CMakeLists.txt中加入如下代码

install(DIRECTORY
  config
  DESTINATION share/${PROJECT_NAME}
)

与此同时在launch启动脚本中添加参数依赖

from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    params_file = os.path.abspath(os.path.join(
    get_package_share_directory("publisher_node_pkg"),
    'config',
    'time.yaml'
))
    
    robot_node_1 = Node(
        package="publisher_node_pkg",
        executable="publisher_node",
        parameters=[params_file]
    )
    robot_node_2 = Node(
        package="subscriber_node_pkg",
        executable="subscriber_node"
    )
    
    launch_description = LaunchDescription(
        [robot_node_1, robot_node_2] )
    return launch_description


接下来构建运行，就能看到发布时间间隔变成了1秒，更改文件内的参数值，并重新构造，便可以更改间隔时间。
由于launch文件被安装到了install目录下，因此上面的路径也是对于install下被安装过来的参数文件的，每次修改参数都要重新构造安装。如果我们更改路径，就能做到直接指向发布者节点内的原参数文件。将路径部分改成下列代码：

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
        'time.yaml'  
    ))





    写在最后：（建议看完所有代码时读）

    ROS的本质就是实现了两个不同的进程之间的信息交流，所以程序还是该怎么写就怎么写，最后再把ROS的框架套上去就行了，所以这就是为司马我i不建议去看网课，它其实非常简单，但是可以玩出很多花样，可以试着去写一点自己的ROS项目（或者把原来的项目砍成两半，一半在发布者那里，然后将参数发到接受者那里完成剩下的工作）但是写作方法不太一样，尝试以赋值粘贴我这里的代码为主（当然，最好是你也写一个自己的模板，这个工程实际上是我写给自己的模板，所以什么功能都会有）以赋值粘贴模板里面的代码为主会让你节省大部分时间，方便流出更多时间到问题的解决上面

   
   
   
                        我竟然写完了  我真厉害 ！！！！