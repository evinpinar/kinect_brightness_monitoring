// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <k4a/k4a.h>
#include <math.h>
#include <list>

using namespace cv;

void detect_brightness(cv::Mat& frame, double& brightness){
    printf("within brightness %d %d \n", frame.rows, frame.cols);
    //Scalar m = cv::mean(frame);
    //brightness = mean(frame)[0];
    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    brightness = mean(hsv)[2];
}

void calculate_histogram(cv::Mat& frame){
    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    int v_bins = 50;
    int histSize[] = { v_bins };

    float v_ranges[] = { 0, 255 };
    const float* ranges[] = { v_ranges };
    int channels[] = { 0 };

    MatND hist;
    std::vector<cv::Mat> channel(3);
    split(hsv, channel);

    calcHist( &channel[2], 1, channels, Mat(), hist, 1, histSize, ranges, true, false );

    cv::Scalar mean, stddev;
    cv::meanStdDev(hist, mean, stddev);

    std::cout << "Mean: " << mean[0] << "   StdDev: " << stddev[0] << std::endl;
    //spdlog::info("Histogram Calculation, mean: {0} stddev: {1}", mean[0], stddev[0]);

}


int main(int argc, char **argv){

    enum enumer {SIFIR, BIR};
    printf("enum: %d %d \n", SIFIR, BIR);

    int returnCode = 1;
    k4a_device_t device = NULL;
    const int32_t TIMEOUT_IN_MS = 1000;
    int captureFrameCount;
    k4a_capture_t capture = NULL;

    captureFrameCount = 100;
    printf("Capturing %d frames\n", captureFrameCount);

    uint32_t device_count = k4a_device_get_installed_count();

    if (device_count == 0)
    {
        printf("No K4A devices found\n");
        return 0;
    }

    if (K4A_RESULT_SUCCEEDED != k4a_device_open(K4A_DEVICE_DEFAULT, &device))
    {
        printf("Failed to open device\n");
        return 0;
    }

    k4a_device_configuration_t config;
    config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    config.color_resolution = K4A_COLOR_RESOLUTION_2160P;
    config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
    config.camera_fps = K4A_FRAMES_PER_SECOND_30;

    if (K4A_RESULT_SUCCEEDED != k4a_device_start_cameras(device, &config))
    {
        printf("Failed to start device\n");
        return 0;
    }

    k4a_color_control_mode_t color_mode;
    int32_t control_value;

    if (K4A_RESULT_SUCCEEDED != k4a_device_get_color_control(device, K4A_COLOR_CONTROL_BRIGHTNESS, &color_mode, &control_value))
    {
        printf("Failed to start device\n");
        return 0;
    }else{
        printf("K4A_COLOR_CONTROL_BRIGHTNESS Mode: %d Value: %d \n", color_mode, control_value);
    }

    k4a_device_get_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, &color_mode, &control_value);
    printf("K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE Mode: %d Value: %d \n", color_mode, control_value);

    k4a_device_get_color_control(device, K4A_COLOR_CONTROL_AUTO_EXPOSURE_PRIORITY, &color_mode, &control_value);
    printf("K4A_COLOR_CONTROL_AUTO_EXPOSURE_PRIORITY priority Mode: %d Value: %d \n", color_mode, control_value);

    k4a_device_get_color_control(device, K4A_COLOR_CONTROL_CONTRAST, &color_mode, &control_value);
    printf("K4A_COLOR_CONTROL_CONTRAST priority Mode: %d Value: %d \n", color_mode, control_value);

    k4a_device_get_color_control(device, K4A_COLOR_CONTROL_SATURATION, &color_mode, &control_value);
    printf("K4A_COLOR_CONTROL_SATURATION priority Mode: %d Value: %d \n", color_mode, control_value);

    k4a_device_get_color_control(device, K4A_COLOR_CONTROL_SHARPNESS, &color_mode, &control_value);
    printf("K4A_COLOR_CONTROL_SHARPNESS priority Mode: %d Value: %d \n", color_mode, control_value);

    k4a_device_get_color_control(device, K4A_COLOR_CONTROL_WHITEBALANCE, &color_mode, &control_value);
    printf("K4A_COLOR_CONTROL_WHITEBALANCE priority Mode: %d Value: %d \n", color_mode, control_value);

    k4a_device_get_color_control(device, K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION, &color_mode, &control_value);
    printf("K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION priority Mode: %d Value: %d \n", color_mode, control_value);

    k4a_device_get_color_control(device, K4A_COLOR_CONTROL_GAIN, &color_mode, &control_value);
    printf("K4A_COLOR_CONTROL_GAIN priority Mode: %d Value: %d \n", color_mode, control_value);

    k4a_device_get_color_control(device, K4A_COLOR_CONTROL_POWERLINE_FREQUENCY, &color_mode, &control_value);
    printf("K4A_COLOR_CONTROL_POWERLINE_FREQUENCY priority Mode: %d Value: %d \n", color_mode, control_value);


    // Exposure mapping: https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/src/color/color_priv.h
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, 33330);
    k4a_device_set_color_control(device, K4A_COLOR_CONTROL_WHITEBALANCE, K4A_COLOR_CONTROL_MODE_MANUAL, 5000);

    //k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_AUTO, 0);
    //k4a_device_set_color_control(device, K4A_COLOR_CONTROL_WHITEBALANCE, K4A_COLOR_CONTROL_MODE_AUTO, 0);

    double curr_brightness, prev_brightness, target_min_brightness, target_max_brightness;
    prev_brightness = 0;
    target_min_brightness = 100;
    target_max_brightness = 160;
    int curr_exposure;
    curr_exposure= 5;
    int device_exposure_mapping []  = {500, 1250, 2500, 8330, 16670, 33330, 41670, 50000, 66670, 83330, 100000, 116670, 13330};

    while (captureFrameCount-- > 0)
    {
        k4a_image_t image;

        // Get a depth frame
        switch (k4a_device_get_capture(device, &capture, TIMEOUT_IN_MS))
        {
        case K4A_WAIT_RESULT_SUCCEEDED:
            break;
        case K4A_WAIT_RESULT_TIMEOUT:
            printf("Timed out waiting for a capture\n");
            continue;
            break;
        case K4A_WAIT_RESULT_FAILED:
            printf("Failed to read a capture\n");
            return 0;
        }

        printf("Capture");

        // Probe for a color image
        image = k4a_capture_get_color_image(capture);
        if (image)
        {
	    
            printf(" | Color res:%4dx%4d stride:%5d ",
                   k4a_image_get_height_pixels(image),
                   k4a_image_get_width_pixels(image),
                   k4a_image_get_stride_bytes(image));

            printf(" Exposure: %4ld ", k4a_image_get_exposure_usec(image));
            printf(" ISO: %4d", k4a_image_get_iso_speed(image));
            printf(" White Balance: %4d \n", k4a_image_get_white_balance(image));

            uint8_t* buffer = k4a_image_get_buffer(image);
            // convert the raw buffer to cv::Mat
            int rows = k4a_image_get_height_pixels(image);
            int cols = k4a_image_get_width_pixels(image);
            cv::Mat colorMat(rows , cols, CV_8UC4, (void*)buffer, cv::Mat::AUTO_STEP);
            detect_brightness(colorMat, curr_brightness);
            printf(" Brigthness: %4f \n", curr_brightness);
            calculate_histogram(colorMat);

            // If it is the first frame, initiate prev brightness
            if(prev_brightness==0){
                prev_brightness = curr_brightness;
            }

            // Update exposure
            if(curr_brightness<target_min_brightness && curr_exposure < 11 ){
                k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, curr_exposure+1);
            }else if(curr_brightness>target_max_brightness && curr_exposure >0 ){
                k4a_device_set_color_control(device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, curr_exposure-1);
            }

            prev_brightness = curr_brightness;

            k4a_image_release(image);
        }
        else
        {
            printf(" | Color None                       \n");
        }

        // release capture
        k4a_capture_release(capture);
        fflush(stdout);
    }

    returnCode = 0;
}
