#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"  //依赖
#include "msg_pkg/msg/mymsg.hpp"    //自定义消息头文件

using std::placeholders::_1;  // 不知道干啥的，加上准没错

// 和发布者类似，创建一个接受者


class SimpleSubscriber : public rclcpp::Node
{
    public:
        SimpleSubscriber() : Node("simple_subscriper")
        {
            subscription_ = this->create_subscription<msg_pkg::msg::Mymsg>(     //依旧指针创建接受者 ， tpoic为laoda，队列长度32
                "laoda", 32, std::bind(&SimpleSubscriber::topic_callback, this, _1));   //捆绑回调函数，收到消息时就调用，剩下两个参数照抄即可
            RCLCPP_INFO(this->get_logger(),"Subscriber noude has been started."); //发布日至
        }
    private:
        void topic_callback(const msg_pkg::msg::Mymsg::SharedPtr msg) const //回调函数，参数是受到的消息指针（别问我为什么是这个格式，我也不知道）
        {
            RCLCPP_INFO(this->get_logger(),"running");
            RCLCPP_INFO(this->get_logger(),"%s %s. I am %s number%d ",
            msg->words.c_str(),msg->say.c_str(),msg->name.c_str(),msg->bmi);//回调函数可以自由发挥，这里只是简单打印
        }

        rclcpp::Subscription<msg_pkg::msg::Mymsg>::SharedPtr subscription_;   //创建那个诡异的接受者指针
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc,argv); //初始化
    rclcpp::spin(std::make_shared<SimpleSubscriber>()); // 运行
    rclcpp::shutdown(); // 关闭
    return 0 ;
}