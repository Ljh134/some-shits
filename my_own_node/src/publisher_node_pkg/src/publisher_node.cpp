#include "rclcpp/rclcpp.hpp"           //ROS2的核心库
#include "std_msgs/msg/string.hpp"     //这个是std_msgs包下的string消息类型的头文件
#include "msg_pkg/msg/mymsg.hpp"        //这里要包含自定义消息接口的头文件（依葫芦画瓢即可）


using namespace std::chrono_literals;


// ========================================================
//核心内容：创建一个发布者节点（不要的功能自行掠过）

//顺带一提：msg包和.yaml文件有着诡异的格式要求，尽量照着例子写

//=========================================================


/*要创建一个发布者，需要使用发布者类，这里使用了一个定时器，每到固定时间就会发布一个消息，当然也可以发布其他函数的运行结果（peixun2项目）*/
//由于ROS2语法过于复杂，建议依葫芦画瓢即可

class SimplePublisher : public rclcpp::Node   //类的继承
{
    public:
      SimplePublisher() : Node("simple_publisher"),count_(0)
      {
          publisher_ = this->create_publisher<msg_pkg::msg::Mymsg>("laoda", 32); //创建一个发布自定义消息的发布者，topic为laoda，队列长度32
          //这个publisher_是一个指针，指向发布者这个对象（定义在下面）作用是方便回调函数使用
          //至于为什么要这样表示msg，我也不知道， 照着写就对了

          //这里是参数传递的部分
          this->declare_parameter<int>("duringtime", 10);  // 这里声明了一个参数， 并提供了默认值10
          this->declare_parameter<double>("mynum",1.0);   //通上
          
          
          int time = this->get_parameter("duringtime").as_int();   // 这里获取参数值，ROS2通过参数名称查找并获取参数，后面相当于类型转换
          double mynum = this->get_parameter("mynum").as_double();  //一样
          

            //定时器部分

          timer_ = this->create_wall_timer(    //创建一个定时器
            std::chrono::milliseconds(time),
             std::bind(&SimplePublisher::timer_callback,this)  //捆绑函数：当定时器到时就调用它
             );
          RCLCPP_INFO(this->get_logger(),"Publisher node has been started,spaced time is %d ms",time); //这个是发布日志信息（浅显的可以认为是cout，但实际上会有很大区别）
          RCLCPP_INFO(this->get_logger(),"mynum is %f",mynum);//一样

          RCLCPP_INFO(this->get_logger(),"mynum is %f",mynum);//一样
        
          
      }
    private:
      void timer_callback() //计时器的回调函数
      {

        //消息接口部分
        //（这里使用了自定义的消息类型，也可以使用标准消息类型，比如std_msgs/msg/String）
        auto message = msg_pkg::msg::Mymsg();  //这里创建了一个自定义消息类型的变量，使用auto关键字来安排类型（不然你也不知道时什么）
        //标准消息类型： auto message = std_msgs::msg::String();
        //然后和下面的差不多

        //message.words = mystr;
        message.num = count_++;  //这个消息变量可以看成一个结构体，成员就是自己定义的，以下为填充消息内容
        message.bmi = 24;
        message.say = "Man! What can I say?";
        message.name = "Kobe Bryant";
        message.words = "Mamba out";
        publisher_->publish(message);  // 这是发布消息
        RCLCPP_INFO(this->get_logger(),"牢大被肘了 %d 次，%d  %s", // 日志输出发布的内容
        message.num,message.bmi,message.name.c_str());
        
      }
      rclcpp::TimerBase::SharedPtr timer_;    // 创建一个定时器指针（其实定时器什么的我没有搞很清楚，干就完事了）
      rclcpp::Publisher<msg_pkg::msg::Mymsg>::SharedPtr publisher_;  //创建一个发布者指针，注意这里的消息类型要和上面的一样
      size_t count_; // 顺手定义一个变量用来计数
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv); //初始化
  rclcpp::spin(std::make_shared<SimplePublisher>()); //运行节点，这里使用了智能指针
  rclcpp::shutdown(); // 关闭节点
  return 0;
}