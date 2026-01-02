#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "msg_pkg/msg/mymessage.hpp"
#include <opencv2/opencv.hpp>
#include<iostream>
#include<vector>

using namespace std::chrono_literals;
using namespace std;
using namespace cv;
using std::placeholders::_1;
Mat result , result1;


class SimpleSubscriber : public rclcpp::Node
{
public:



  SimpleSubscriber() : Node("simple_subscriber")
  {
    subscription_ = this->create_subscription<msg_pkg::msg::Mymessage>(
      "pos", 512, std::bind(&SimpleSubscriber::topic_callback, this, _1));
    
    RCLCPP_INFO(this->get_logger(), "订阅者节点已启动");
  }


  void topic_callback(const msg_pkg::msg::Mymessage::SharedPtr msg) const
  { 
    int center_x , center_y;
    int rect_x , rect_y ,rect_w,rect_h;
    int number;
    double rect_x1 , rect_y1 ,rect_w1,rect_h1;

    RCLCPP_INFO(this->get_logger(), "收到消息");
    number = msg->tol ;
    int pos[400][2] = {0} ;
    int rect1[400][4] = {0};
    double res_rect[12][4] = {0};
    for (int i = 1; i < number && i < (int)msg->pos.size(); i++)
    {
      
      const auto &p = msg->pos[i].data;
      if (p.size() >= 2) {
        pos[i][0] = p[0];
        pos[i][1] = p[1];
      } else {
        pos[i][0] = pos[i][1] = 0;
      }

      if (i < (int)msg->show.size()) {
        const auto &r = msg->show[i].data;
        if (r.size() >= 4) {
          rect1[i][0] = r[0];
          rect1[i][1] = r[1];
          rect1[i][2] = r[2];
          rect1[i][3] = r[3];
        } else {
          rect1[i][0] = rect1[i][1] = rect1[i][2] = rect1[i][3] = 0;
        }
      } else {
        rect1[i][0] = rect1[i][1] = rect1[i][2] = rect1[i][3] = 0;
      }
      
    }
    for (int j = 0; j < (int)msg->res.size(); j++)
  {
    const auto &l = msg->res[j].data;
    if(l[0])
    {
      res_rect[j][0] = l[0];
      res_rect[j][1] = l[1];
      res_rect[j][2] = l[2];
      res_rect[j][3] = l[3];
    }
    else {
      res_rect[j][0] = res_rect[j][1] = res_rect[j][2] = res_rect[j][3] = 0 ;
      
    }
  }

    result = Mat::zeros(480,852, CV_8UC1); 
    result1 = Mat::zeros(480,852, CV_8UC1); 
    namedWindow("result", WINDOW_NORMAL);
    resizeWindow("result",1600,1200);
    namedWindow("result1", WINDOW_NORMAL);
    resizeWindow("result1",1600,1200);
    for(int i = 1 ; i <= number ; i++)
    {
      if(pos[i][0] && pos[i][1] && rect1[i][0] && rect1[i][1] 
        &&rect1[i][2] && rect1[i][3])
        {
          center_x = pos[i][0];
          center_y = pos[i][1];
          rect_x = rect1[i][0];
          rect_y = rect1[i][1];
          rect_w = rect1[i][2];
          rect_h = rect1[i][3];
          circle(result, Point(center_x, center_y), 2, Scalar(255, 255, 255), 2, 8, 0);
          Rect rect(rect_x, rect_y, rect_w, rect_h);
          rectangle(result, rect, Scalar(255,255,255), 1.5, 8, 0);
          putText(result, format("%d", i), Point(center_x, center_y),
          FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
        }
    }
    for(int k = 0 ; k< msg->count ; k++)
    {
          rect_x1 = res_rect[k][0];
          rect_y1 = res_rect[k][1];
          rect_w1 = res_rect[k][2];
          rect_h1 = res_rect[k][3];
          Rect rect1(rect_x1, rect_y1, rect_w1, rect_h1);
          rectangle(result1, rect1, Scalar(255,255,255), 1.5, 8, 0);
    }
    imshow("result", result);
    imshow("result1", result1);
    waitKey(1000/30);
    RCLCPP_INFO(this->get_logger(), "运行成功");
  }
  
  rclcpp::Subscription<msg_pkg::msg::Mymessage>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SimpleSubscriber>());
  rclcpp::shutdown();
  return 0;
}
