#include <iostream>
#include <string>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
using namespace std;

char window_title[] = "Display Image";

int main(int argc, char** argv)
{
    if ( argc < 2 )
    {
        cout << "Usage: " << argv[0] << " IMAGE_FILE" << endl;
        return -1;
    }

    cv::Mat image = cv::imread(argv[1]);

    if( !image.data )
    {
        cout << "No image data" << endl;
        return -2;
    }

    cv::namedWindow(window_title, CV_WINDOW_AUTOSIZE);
    cv::imshow(window_title, image);

    cv::waitKey(0);

    return 0;
}
