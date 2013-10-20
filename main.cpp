#include <iostream>
#include <string>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
using namespace std;

string window_title = "Sartroscope show image";
string output_filename;

// From http://www-personal.umich.edu/~shameem/haarcascade_eye.html
string eyes_cascade_name = "haarcascade_eye.xml";
string face_cascade_name = "haarcascade_frontalface_alt.xml";
string nose_cascade_name = "haarcascade_nose.xml";

cv::CascadeClassifier face_cascade;
cv::CascadeClassifier eyes_cascade;
cv::CascadeClassifier nose_cascade;
cv::RNG rng(12345);

void detectAndDisplay(cv::Mat frame, cv::Mat background)
{
    // prepare given image for eye detection
    cv::Mat detection_frame;
    cv::cvtColor(frame, detection_frame, CV_BGR2GRAY);
    cv::equalizeHist(detection_frame, detection_frame);

    // prepare result image
    cv::Mat grayscale_frame;
    cv::cvtColor(frame, grayscale_frame, CV_BGR2GRAY);

    // blend with background
    cv::cvtColor(background, background, CV_BGR2GRAY);
    cv::Mat backgr_part = background(cv::Rect(0, 0, frame.cols, frame.rows));
    cv::addWeighted(grayscale_frame, 0.8, backgr_part, 0.2, 0.0, grayscale_frame);

    // detect eyes
    std::vector<cv::Rect> eyes;
    eyes_cascade.detectMultiScale(frame, eyes, 1.2, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );

    for(int i = 0; i < eyes.size(); i++)
    {
       cv::Point center(eyes[i].x + eyes[i].width*0.5, eyes[i].y + eyes[i].height*0.5);
       int radius = cvRound( (eyes[i].width + eyes[i].height)*0.10 );
       cv::circle(grayscale_frame, center, radius, cv::Scalar( 255, 255, 255 ), -1, 8, 0 );
    }

    cv::Mat gaussian_noise_frame = grayscale_frame.clone();
    cv::randn(gaussian_noise_frame, 128, 30);

    cv::Mat noised_grframe;
    cv::addWeighted(grayscale_frame, 0.8, gaussian_noise_frame, 0.2, 0.0, noised_grframe);

    cv::Mat smoothed_ngrframe;
    cv::GaussianBlur(noised_grframe, smoothed_ngrframe, cv::Size(3, 3), 0, 0);

    if ( output_filename == "" )
    {
        cv::imshow(window_title, smoothed_ngrframe);
        cv::waitKey(0);
    }
    else
    {
        // write result to file
        cout << "Writing to \"" << output_filename << "\"... ";

        cv::imwrite(output_filename, smoothed_ngrframe);
        cout << "OK!" << endl;
    }
}

int main(int argc, char** argv)
{
    if ( argc > 4 )
    {
        cout << "Usage: " << argv[0] << " IMAGE_FILE BACKGROUND_FILE [OUTPUT_FILE]" << endl;
        return -1;
    }

    if ( argc == 4 )
        output_filename = argv[3];

    cv::Mat image      = cv::imread(argv[1]);
    cv::Mat background = cv::imread(argv[2]);

    if( !image.data || !background.data )
    {
        cout << "No image or background data" << endl;
        return -2;
    }

    // Load Haar cascades
    if ( !eyes_cascade.load(eyes_cascade_name) )
    {
        cout << "!! -- Error loading Haar cascades!" << endl;
        return -3;
    }

    detectAndDisplay(image, background);
    return 0;
}
