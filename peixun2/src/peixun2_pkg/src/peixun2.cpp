#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include "rclcpp/rclcpp.hpp"
#include "msg_pkg/msg/mymessage.hpp"
#include <cstring>

using namespace std::chrono_literals;
using namespace std;
using namespace cv;
namespace fs = std::filesystem;

//configs

    fs::path base_path = "../resource";
    fs::path file_name = "twoRobot.png";
    fs::path video_name = "robotvideo2.mp4";
    fs::path file_path = base_path / file_name;
    fs::path video_path = base_path / video_name;

    int value;
    Mat imghsv,mask;
    static Mat imgr, imgr1;
    Mat imgh , imgs ,imgv ;
    Mat imgh1 ,imgs1 , imgv1;
    vector<Mat> HSV ;
    Mat imges[3];
    Mat img ;
    Mat middle , process_result;
    

    struct object
    {
        double pos_x;
        double pos_y;
        double rect1_x;
        double rect1_y;
        double rect1_w;
        double rect1_h;
    };


    // ============ callback functions ============
        static void callBack1(int, void*)
    {
        int cur = 190;
        //cur = getTrackbarPos("red_value", "red_mask");
        threshold(imgr, imgr1, cur, 255, THRESH_BINARY);
        imshow("red_mask", imgr1);
    }

    static void callBack2(int ,void*)
    {
        int cur = 115;
        //cur = getTrackbarPos("s_value", "s_mask");
        threshold(imgs,imgs1 , cur , 255, THRESH_BINARY);
        imshow("s_mask", imgs1);
    }

    static void callBack3(int ,void*)
    {
        int cur = 150;
        //cur = getTrackbarPos("v_value", "v_mask");
        threshold(imgv,imgv1 , cur , 255, THRESH_BINARY);
        imshow("v_mask", imgv1);
    }

   
    object allObject[400] = {};
    
    double max(double a,double b)
    {
        if (b>= a )
        {
            a = b ;
        }
        return a;
    }
    double min(double a, double b )
    {
        if (b >= a)
        {
            b = a ;
        }
        return b ;
    }

    void freearray(double** arr)
    {
        for (int i = 0; i< 12;++i)
        {
            delete[] arr[i];
        }
        delete[] arr;
    }


    // ===================== define function to find the lights ======================
    //it define the lights should meet the requirements and return the arrey of lights' position
    double** def()
    {
       double ** ban = new double*[12];
       for(int m = 0;m<12;m++)
       {
        ban[m] = new double[4]();
       } 
       for (int p = 1; p<400;p++)
       {
            double wh1 = allObject[p].rect1_h / allObject[p].rect1_w;
            if(wh1 >= 1.0 && wh1 <= 3.5)
            {
                for (int q = 1; q<p;q++)
                {
                    
                    double wh2 = allObject[q].rect1_h / allObject[q].rect1_w;
                    if(wh2 >= 1.0 && wh2 <= 3.5)
                    {
                        double dx = allObject[p].pos_x - allObject[q].pos_x;
                        if(dx < 0)
                        {
                            dx = -dx ;
                        }
                        double dy1 = dx / allObject[p].rect1_w;
                        double dy2 = dx / allObject[q].rect1_w;
                        if(dy1 <= 5 && dy1 >= 1.2 && dy2 <= 5 && dy2 >= 1.2)
                        {
                            int count = 0 ;
                            if (count < 12)
                            {
                            double dy = max(allObject[p].rect1_h,allObject[q].rect1_h);
                            double x1 = min(allObject[p].rect1_x,allObject[q].rect1_x);
                            double y1 = min(allObject[p].rect1_y,allObject[q].rect1_y);
                            ban[count][0] = x1;
                            ban[count][1] = y1;
                            ban[count][2] = dx;
                            ban[count][3] = dy;
                           
                            count ++;
                            }
                        }

                    }
                }
            }
        }
        return ban ; 
    }


// ======================== here is the publishger framework  ============================

// the inheritance from rclcpp::Node
class SimplePublisher : public rclcpp::Node
{
public:

// ================= video main function ===================
//actually this is a wrong way to put the function here , but I don't wanna change too much code structure

int videomain()
    {
    
    /*
    Mat img = imread(file_path.string());
    if(img.empty())
    {
        cout << "确认文件名是否正确" << endl;
        return -1 ;
    }
        */
    //imshow("img",img);
    VideoCapture video(video_path);
    if(!video.isOpened())
    {
        cout << "确认文件名"
        <<endl;
        return -1;
    }

    {
        video >> img;
        if(img.empty())
        {
            cout << "video is empty , exit now" <<endl;
            
        }
        
        {
        cvtColor(img ,imghsv ,COLOR_RGB2HSV);
        split(imghsv , HSV);
        imgh = HSV.at(0);
        imgs = HSV.at(1);
        imgv = HSV.at(2);
        split(img, imges);
        imgr = imges[0];
        }
    }

    {
    namedWindow("red_mask", WINDOW_NORMAL);
    threshold(imgr, imgr1, 128, 255, THRESH_BINARY);
    imshow("red_mask", imgr1);
    createTrackbar("red_value", "red_mask", nullptr, 255, callBack1);

    namedWindow("s_mask", WINDOW_NORMAL);
    threshold(imgs , imgs1, 128,255, THRESH_BINARY);
    imshow("s_mask", imgs1);
    createTrackbar("s_value", "s_mask", nullptr, 255, callBack2);

    namedWindow("v_mask", WINDOW_NORMAL);
    threshold(imgv , imgv1, 128 ,255, THRESH_BINARY);
    imshow("v_mask" , imgv1);
    createTrackbar("v_value", "v_mask", nullptr, 255, callBack3);

    namedWindow("mask", WINDOW_NORMAL);
    resizeWindow("mask",1600,1200);
    namedWindow("video",WINDOW_NORMAL);
    resizeWindow("video",1200,900);
    }
    auto message = msg_pkg::msg::Mymessage();
    while(1)
    {
        
        video >> img;
        if(img.empty())
        {
            cout << "over"
            <<endl;
            break;
        }

        {
        cvtColor(img ,imghsv ,COLOR_BGR2HSV);
        split(imghsv , HSV);
        imgh = HSV.at(0);
        imgs = HSV.at(1);
        imgv = HSV.at(2);
        split(img, imges);
        imgr= imges[2];
        }

        
        callBack1(0,0);
        callBack2(0,0);
        callBack3(0,0);
        mask = imgr1 & imgs1 & imgv1;
        
        
    
        Mat kernel = getStructuringElement(0, Size(3, 3));
        Mat kernel2 = getStructuringElement(0, Size(8, 8));
        morphologyEx(mask, middle, MORPH_OPEN, kernel);
        morphologyEx(middle, process_result, MORPH_CLOSE, kernel2);
        namedWindow("运算处理结果", WINDOW_NORMAL);  
        imshow("运算处理结果", process_result);
        RNG rng(10086);
        Mat out, stats, centroids;
        int number = connectedComponentsWithStats(process_result, out, stats, centroids, 8, CV_16U);
        vector<Vec3b> colors;
        for (int i = 0; i < number && i< 399; i++)
        {
            Vec3b vec3 = Vec3b(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
            colors.push_back(vec3);
        }

        Mat result = Mat::zeros(process_result.size(), img.type());
        
        memset(allObject, 0, sizeof(allObject));
        for (int i = 1; i < number && i<399; i++)
        {
            int center_x = centroids.at<double>(i, 0);
            int center_y = centroids.at<double>(i, 1);
            int rect_x = stats.at<int>(i, CC_STAT_LEFT);
            int rect_y = stats.at<int>(i, CC_STAT_TOP);
            int rect_w = stats.at<int>(i, CC_STAT_WIDTH);
            int rect_h = stats.at<int>(i, CC_STAT_HEIGHT);
            int area = stats.at<int>(i, CC_STAT_AREA);

            allObject[i].pos_x = center_x;
            allObject[i].pos_y = center_y;
            allObject[i].rect1_x = rect_x;
            allObject[i].rect1_y = rect_y;
            allObject[i].rect1_w = rect_w;
            allObject[i].rect1_h = rect_h;

            

            circle(img, Point(center_x, center_y), 2, Scalar(0, 255, 0), 2, 8, 0);
            Rect rect(rect_x, rect_y, rect_w, rect_h);
            rectangle(img, rect, colors[i], 1.5, 8, 0);
            putText(img, format("%d", i), Point(center_x, center_y),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
            cout << "number: " << i << ",area: " << area << endl;
            circle(mask, Point(center_x, center_y), 2, Scalar(0, 255, 0), 2, 8, 0);
            rectangle(mask, rect, Scalar(255,255,255), 1.5, 8, 0);
            putText(mask, format("%d", i), Point(center_x, center_y),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

            message.info = "target number" ;
            message.num = i ;
            message.pos[i].data = {{center_x,center_y}};
            message.show[i].data = {{rect_x, rect_y, rect_w, rect_h}};
            

        }


        
        imshow("video", img);
        imshow("mask", mask); 

        // =================== video processing finished , now poblish the message ========
        

        // ======================== get the lights ===========================================
        double** ban1 = def();
        int cou = 0;
        for (int i = 0 ; i< 12 ; i ++)
        {   
            
            if(ban1[i][0]){
            message.res[i].data = {{ban1[i][0],ban1[i][1],ban1[i][2],ban1[i][3]}};
            cou++;
            }
        }
        freearray(ban1);

//======================= fill the message =========================
        
        message.tol = number;
        message.count = cou;

        // publish the message
        publisher_->publish(message);

        //sleep
        //waitKey(1000/ video.get(CAP_PROP_FPS));
        int delay = static_cast<int>(1000.0 / video.get(CAP_PROP_FPS));
        //get the log info
        RCLCPP_INFO(this->get_logger(), "发布消息成功");

        int key = waitKey(delay >= 1 ? delay : 1);

        if (key == 27) { 
            cout << "ESC pressed, exiting playback" << endl;
           break;
        }

    }
    video.release();
    destroyAllWindows();
    return 0;
    }

    //create a pobulisher node
    SimplePublisher() : Node("simple_publisher"), count_(0)
    {
        //here is the function run while the initialization of node

        //"pos" is the topic name , 512 is the queue size
        publisher_ = this->create_publisher<msg_pkg::msg::Mymessage>("pos",512);

        //timer_ = this->create_wall_timer(500ms, std::bind(&SimplePublisher::timer_callback, this));
        
        
        RCLCPP_INFO(this->get_logger(), "发布者节点已启动");
        videomain();
        RCLCPP_INFO(this->get_logger(), "运行结束");
    }

/*
  
  void timer_callback()
  {
    // 创建消息对象
    auto message = std_msgs::msg::String();
    message.data = "RUNS!!! " + std::to_string(count_++);
    // 发布消息
    publisher_->publish(message);
    // 打印日志信息
    RCLCPP_INFO(this->get_logger(), "发布消息: '%s'", message.data.c_str());
  }

  */

  //create a shared timer pointer and a shared publisher pointer and a counter(actually they can be private)
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<msg_pkg::msg::Mymessage>::SharedPtr publisher_;
  size_t count_;
};

   

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  
  //run the publisher node
  rclcpp::spin(std::make_shared<SimplePublisher>());
  //shutdown the ros2 system
  rclcpp::shutdown();
  return 0;
}
