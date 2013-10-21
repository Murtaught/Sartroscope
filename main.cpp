#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <string>
#include <vector>
using namespace std;

string window_title = "Sartroscope show image";
string output_filename;

// From http://www-personal.umich.edu/~shameem/haarcascade_eye.html
string eyes_cascade_name = "haarcascade_eye.xml";

cv::CascadeClassifier eyes_cascade;
cv::RNG rng(12345);

// frame and background expected to be monochrome
void process(cv::Mat frame, cv::Mat background)
{
    // prepare result image
    cv::Mat result = frame.clone();

    // blend with background
    cv::Mat backgr_part = background(cv::Rect(0, 0, frame.cols, frame.rows));
    cv::addWeighted(result, 0.8, backgr_part, 0.2, 0.0, result);

    // detect eyes
    std::vector<cv::Rect> eyes;
    eyes_cascade.detectMultiScale(frame, eyes, 1.2, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );

    // draw scary eyes
    for(int i = 0; i < eyes.size(); i++)
    {
       cv::Point center(eyes[i].x + eyes[i].width  * 0.5,
                        eyes[i].y + eyes[i].height * 0.5);
       int radius = cvRound( (eyes[i].width + eyes[i].height) * 0.1 );

       // draw filled (-1 arg) white circle with center at 'center'
       cv::circle(result, center, radius, cv::Scalar(255, 255, 255), -1, 8, 0 );
    }

    // add some random noise
    cv::Mat noise(result.size(), result.type());
    cv::randn(noise, 128, 30);
    cv::addWeighted(result, 0.8, noise, 0.2, 0.0, result);

    // add some blur
    cv::GaussianBlur(result, result, cv::Size(3, 3), 0, 0);

    if ( output_filename == "" )
    {
        cv::imshow(window_title, result);
    }
    else
    {
        // write result to file
        cout << "Writing to \"" << output_filename << "\"... ";

        cv::imwrite(output_filename, result);
        cout << "OK!" << endl;
    }
}

int main(int argc, char** argv)
{
    if ( argc > 4 || argc < 3 )
    {
        cout << "Usage: " << argv[0] << " IMAGE_FILE BACKGROUND_FILE [OUTPUT_FILE]" << endl;
        return -1;
    }

    if ( argc == 4 )
        output_filename = argv[3];

    // Load Haar cascades
    if ( !eyes_cascade.load(eyes_cascade_name) )
    {
        cout << "!! -- Error loading Haar cascades!" << endl;
        return -3;
    }

    // Load background file
    cv::Mat background = cv::imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
    if ( !background.data )
    {
        cout << "No background data" << endl;
        return -2;
    }

    if ( string(argv[1]) != "capture" )
    {
        // Disable output file mode if it is enabled
        output_filename = "";

        // Load image file
        cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
        if( !image.data )
        {
            cout << "No image data" << endl;
            return -2;
        }

        process(image, background);

        // If we show result in a window wait for a keypress
        // else exit immediately
        if ( output_filename.empty() )
            cv::waitKey(0);
    }
    else
    {
        // Capture video stream from webcam
        cv:CvCapture *capture;
        cv::Mat frame;

        capture = cvCaptureFromCAM( -1 ); // trying to capture webcam
        if ( capture )
        {
            while (true)
            {
                frame = cvQueryFrame(capture);

                if ( !frame.empty() )
                {
                    process(frame, background);
                }
                else
                {
                    cout << "Captured empty frame, exiting" << endl;
                    break;
                }

                int c = cv::waitKey( 10 );
                if ( char(c) == 'c' )
                {
                    cout << "User pressed 'c', exiting" << endl;
                    break;
                }
            }
        }
        else
        {
            cout << "Failed to capture webcam" << endl;
            return -4;
        }
    }
    return 0;
}
