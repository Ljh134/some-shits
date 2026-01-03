cmake_minimum_required(VERSION 3.8)
project(publisher_node_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

#要使用std_msgs消息类型就要加下面这一行（顺手加了）
find_package(std_msgs REQUIRED)

# 添加可执行文件
add_executable(publisher_node src/publisher_node.cpp)

#找到自定义消息包
find_package(msg_pkg REQUIRED)

#添加依赖（依旧依葫芦画瓢）
ament_target_dependencies(publisher_node rclcpp std_msgs msg_pkg)

#安装可执行文件（依旧依葫芦画瓢）
install(TARGETS
	publisher_node
	DESTINATION lib/${PROJECT_NAME}
)

#安装配置文件（如果需要参数传递的话）（依旧依葫芦画瓢）
install(DIRECTORY
  config
  DESTINATION share/${PROJECT_NAME}
  )
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
