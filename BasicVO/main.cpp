#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>
#include <iomanip>

#include "PinholeCamera.h"
#include "VisualOdometry.h"


int main()
{
    const char *position_file_name = "position.txt";
    const char *frame_viewer_name = "Road facing camera";
    const char *trajectory_viewer_name = "Trajectory";

    const char *image_list_folder = "/home/runisys/Desktop/data/slamDataset/sequences/00/image_0/";

    char text[200];


    PinholeCamera *pcamera = new PinholeCamera(1241, 376, 718.8560, 718.8560, 607.1928, 185.2157);
    VisualOdometry vo(pcamera);

    std::fstream out(position_file_name);
    int font_face = cv::FONT_HERSHEY_PLAIN;
    double font_scale = 1;
    int thickness = 1;
    cv::Point text_org(10, 50);

    cv::namedWindow(frame_viewer_name, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(trajectory_viewer_name, cv::WINDOW_AUTOSIZE);
    
    cv::Mat traj = cv::Mat::zeros(600, 600, CV_8UC3);

    double x = 0, y = 0, z = 0;
    for (int im_id = 0; im_id < 2000; ++im_id)
    {
        std::stringstream ss;
        ss << image_list_folder << std::setw(6) << std::setfill('0') << im_id
           << ".png";

        cv::Mat img = cv::imread(ss.str().c_str(), 0);
        assert(!img.empty());

        vo.addImage(img, im_id);
        cv::Mat cur_t = vo.getCurrentT();
        if (cur_t.rows != 0)
        {
            x = cur_t.at<double>(0);
            y = cur_t.at<double>(1);
            z = cur_t.at<double>(2);
        }
        out << x << " " << y << " " << z << std::endl;

        int draw_x = int(x) + 300;
        int draw_y = int(z) + 100;
        cv::circle(traj, cv::Point(draw_x, draw_y), 1, CV_RGB(255, 0, 0), 2);

        cv::rectangle(traj, cv::Point(10, 30), cv::Point(580, 60), CV_RGB(0, 0, 0), CV_FILLED);
        sprintf(text, "Coordinates: x = %02fm y = %02fm z = %02fm", x, y, z);
        cv::putText(traj, text, text_org, font_face, font_scale, cv::Scalar::all(255), thickness, 8);

        cv::imshow(frame_viewer_name, img);
        cv::imshow(trajectory_viewer_name, traj);

        cv::waitKey(1);
    }
    delete pcamera;
    out.close();

    return 0;
}