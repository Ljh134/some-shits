#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <filesystem>

using namespace std::chrono_literals;
using namespace std;
using namespace cv;
namespace fs = std::filesystem;

// here is the main config area

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


// callback function area

//================== README ===================
//  if you want to adjust the configuration (threshould value) of each channel cancel the comment of the following lines noted by "config: adjust"
// and commant the default value in the callback function (noted by "config: default value")
// if you want this program run steadily , commant the following lines note by "config : adjust" ,but using the default value 

        static void callBack1(int, void*)
    {

        //config : default value
        int cur = 90;
        //config : adjust
        //cur = getTrackbarPos("red_value", "red_mask");
        threshold(imgr, imgr1, cur, 255, THRESH_BINARY);
        imshow("red_mask", imgr1);
    }

    static void callBack2(int ,void*)
    {
        //config : default value
        int cur = 90;
        //config : adjust
        //cur = getTrackbarPos("s_value", "s_mask");
        threshold(imgs,imgs1 , cur , 255, THRESH_BINARY);
        imshow("s_mask", imgs1);
    }

    static void callBack3(int ,void*)
    {
        //config : default value
        int cur = 50;
        //config : adjust
        //cur = getTrackbarPos("v_value", "v_mask");
        threshold(imgv,imgv1 , cur , 255, THRESH_BINARY);
        imshow("v_mask", imgv1);
    }

    static void callBack4(int ,void*)
    {
        //config : default value
        int cur = 50;
        //config : adjust
        //cur = getTrackbarPos("h_value", "h_mask");
        threshold(imgh,imgh1 , cur , 255, THRESH_BINARY_INV);
        imshow("h_mask", imgh1);
    }
   

    // processing video
    // ========================== README ===============================
    // there are 3 ways to input the video , uncommant the one you want to use
    //1 photo input : input a photo (but is unvalid in this program , if you want you should delete the "while()" function , 
    //                                              and command the function noted by "video process")
    //2 steady video input : input a video file
    //3 video of your camera : input your camera video(maybe not work in your computer , )
    

    int videomain()
    {

    // ===================== photo input ==========================

    /*
    Mat img = imread(file_path.string());
    if(img.empty())
    {
        cout << "确认文件名是否正确" << endl;
        return -1 ;
    }
        */
    //imshow("img",img);

// =====================photo end =====================


//========================== steady video input ============================
    VideoCapture video(video_path);

// ========================= end ========================================= 


// ========================= video of your camera input =============================
   // VideoCapture video(0);
// =========================== end ==============================

// ================= video process ===============================

    if(!video.isOpened())
    {
        cout << "确认文件名"
        <<endl;
        return -1;
    }

    {
        video >> img;
// =========================== end ==================================================
        
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


    //creating trackbars to adjust the threshould value
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

    namedWindow("h_mask", WINDOW_NORMAL);
    threshold(imgh , imgh1, 128,255, THRESH_BINARY_INV);
    imshow("h_mask", imgh1);
    createTrackbar("h_value", "h_mask", nullptr, 255, callBack4);

    namedWindow("mask", WINDOW_NORMAL);
    resizeWindow("mask",1600,1200);
    namedWindow("video",WINDOW_NORMAL);
    resizeWindow("video",1200,900);
    }

    while(1)
    {
        
// here is the main process of a single frame

        video >> img;
        if(img.empty())
        {
            cout << "over"
            <<endl;
            break;
        }

        //color split
        {
        cvtColor(img ,imghsv ,COLOR_BGR2HSV);
        split(imghsv , HSV);
        imgh = HSV.at(0);
        imgs = HSV.at(1);
        imgv = HSV.at(2);
        split(img, imges);
        imgr= imges[2];
        }

        //creat trackbar
        callBack1(0,0);
        callBack2(0,0);
        callBack3(0,0);
        callBack4(0,0);
        mask = imgr1 & imgs1 & imgv1 ;
        
       //calculation of the mask to get every object
    
        Mat kernel = getStructuringElement(0, Size(3, 3));
       // Mat kernel2 = getStructuringElement(0, Size(7, 7));
        morphologyEx(mask, middle, MORPH_OPEN, kernel);
        morphologyEx(middle, process_result, MORPH_CLOSE, kernel);
        namedWindow("运算处理结果", WINDOW_NORMAL);  
        imshow("运算处理结果", process_result);
        RNG rng(10086);
        Mat out, stats, centroids;
        int number = connectedComponentsWithStats(process_result, out, stats, centroids, 8, CV_16U);
        vector<Vec3b> colors;
        for (int i = 0; i < number; i++)
        {
            Vec3b vec3 = Vec3b(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
            colors.push_back(vec3);
        }

        Mat result = Mat::zeros(process_result.size(), img.type());
    
    for (int i = 1; i < number; i++)
    {
        int center_x = centroids.at<double>(i, 0);
        int center_y = centroids.at<double>(i, 1);
        int x = stats.at<int>(i, CC_STAT_LEFT);
        int y = stats.at<int>(i, CC_STAT_TOP);
        int w = stats.at<int>(i, CC_STAT_WIDTH);
        int h = stats.at<int>(i, CC_STAT_HEIGHT);
        int area = stats.at<int>(i, CC_STAT_AREA);

        circle(img, Point(center_x, center_y), 2, Scalar(0, 255, 0), 2, 8, 0);
        Rect rect(x, y, w, h);
        rectangle(img, rect, colors[i], 1.5, 8, 0);
        putText(img, format("%d", i), Point(center_x, center_y),
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
        cout << "number: " << i << ",area: " << area << endl;
        circle(mask, Point(center_x, center_y), 2, Scalar(0, 255, 0), 2, 8, 0);
        rectangle(mask, rect, Scalar(255,255,255), 1.5, 8, 0);
        putText(mask, format("%d", i), Point(center_x, center_y),
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    }


        
        imshow("video", img);
        imshow("mask", mask);

        // here ensure the video play at normal speed
        //
        waitKey(1000/ video.get(CAP_PROP_FPS));
        int delay = static_cast<int>(1000.0 / video.get(CAP_PROP_FPS));
        int key = waitKey(delay >= 1 ? delay : 1);

        if (key == 27) { // ESC
            cout << "ESC pressed, exiting playback" << endl;
           break;
        }

    }
    video.release();
    destroyAllWindows();
    return 0;
    }
    

    int main()
    {
        videomain();
        return 0 ;
    }
