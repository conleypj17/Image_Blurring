#include <stdio.h>
#include <iostream>

//Include OpenCV header files to recognize "Mat"
#include <opencv2/core/core.hpp>  //Define the most important class/structure
#include <opencv2/highgui/highgui.hpp> //provide input/output functions
#include <opencv2/imgproc/imgproc.hpp> //provide image processing functions

using namespace cv;
using namespace std; //cout or cin for c++

Mat image;  //Mat is the most important class in opencv
Mat im;
//"matrix" is considered as an image holder
Mat image_org;  //This image is holding the original values

Point pt = Point(-1, -1);  //Represent previous point
Point selectionEnd;  // Store the bottom-right corner of the selected area

bool drawing = false; //indicator: the mouse is dragging

int blur_degree;    //global variable for how big the blur blocks are

//Dynamic array
unsigned char* arr1D;  //1D dynamic array to hold the data from "image" in OpenCV
unsigned char** arr2D;  //Alternatively, you can use 2D array to operate the image

//using 2D array
void blurBlock(Point p1, Point p2)
{
    int region_width = p2.x - p1.x; //getting the width of the region
    int region_height = p2.y - p1.y; //getting the heigh of the region

    int block_size_x = max(1, blur_degree);  //getting the block sizes based on blur_degree - blur_degree is initially 5
    int block_size_y = max(1, blur_degree);  

    for (int by = p1.y; by < p2.y; by += block_size_y)  //iterating over rows
    {
        for (int bx = p1.x; bx < p2.x; bx += block_size_x)  //iterating over columns
        {
            int sum_r = 0;
            int sum_g = 0;
            int sum_b = 0;
            int count = 0;

            // Compute the average for the current block
            for (int y = by; y < min(by + block_size_y, p2.y); y++)
            {
                for (int x = bx; x < min(bx + block_size_x, p2.x); x++)
                {
                    sum_b += arr2D[y][x * 3 + 0];
                    sum_g += arr2D[y][x * 3 + 1];
                    sum_r += arr2D[y][x * 3 + 2];
                    count++;
                }
            }

            int avg_b = sum_b / count;  //getting the average of each color by dividing the sum by count
            int avg_g = sum_g / count;
            int avg_r = sum_r / count;

            //applying the average to all of the pixels
            for (int y = by; y < min(by + block_size_y, p2.y); y++)
            {
                for (int x = bx; x < min(bx + block_size_x, p2.x); x++)
                {
                    arr2D[y][x * 3 + 0] = avg_b;    //applying the average to blur the blocks
                    arr2D[y][x * 3 + 1] = avg_g;
                    arr2D[y][x * 3 + 2] = avg_r;
                }
            }
        }
    }

    //copying the data back to the image
    for (int y = p1.y; y < p2.y; y++)
    {
        memcpy(image.data + y * image.cols * 3, arr2D[y], image.cols * 3);
    }
}

void onMyMouse(int event, int x, int y, int flag, void* data)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        pt = Point(x, y);
        drawing = true;
    }
    else if (event == EVENT_LBUTTONUP)
    {
        drawing = false;
        selectionEnd = Point(x, y);  //storing the region's endpoints to ensure that the image does not get bigger
        blurBlock(pt, selectionEnd);
        image.copyTo(image_org);
    }

    if (drawing)
    {
        image_org.copyTo(image);
        rectangle(image, pt, Point(x, y), Scalar(0, 255, 255), 3);
    }
}

void resetImage()
{
    im = imread("C:/Users/conle/OneDrive/Pictures/CPS352Image.jpg"); //load image file into RAM and store it as "img" variable
    resize(im, image, Size(1000, 500), 0, 0, INTER_LINEAR);
    image.copyTo(image_org);  //In the beginning, image_original is identical as img2
}   //end resetImage method

int main(int argc, char** argv)
{
    blur_degree = 5;    //initializing blur_degree to 5


    im = imread("C:/Users/conle/OneDrive/Pictures/CPS352Image.jpg"); //load image file into RAM and store it as "img" variable
    resize(im, image, Size(1000, 500), 0, 0, INTER_LINEAR);
    image.copyTo(image_org);  //in the beginning, image_original is identical as img2

    printf("Image size: (%d, %d)\n", image.rows, image.cols);

    /** 1D array creation and assign values from image      */
    arr1D = new unsigned char[image.rows * image.cols * 3]; //allocate space in RAM
    memcpy(arr1D, image.data, image.rows * image.cols * 3); //copy value from OpenCV image


    /** 2D array creation and assign values from image      */
    arr2D = new unsigned char* [image.rows]; //Allocate rows or 1st dimension
    for (int y = 0; y < image.rows; y++)
    {
        arr2D[y] = new unsigned char[image.cols * 3]; //allocate space in RAM cols or 2nd dimension
        memcpy(arr2D[y], image.data + y * image.cols * 3, image.cols * 3);
    }


    namedWindow("My Window"); //Create a window called "My Window"
    // This line is optional if you just display an image in the window
    //But if you need the window receive mouse event, you have to declare this window first

    setMouseCallback("My Window", onMyMouse); //register the callback function


    while (1)
    {
        imshow("My Window", image); //OpenCV create a window called "My Window"
        // display the img within the "My Window"


        //Using keyboard to stop the while loop
        char c = waitKey(100);

        if (c == 27) //ASCII code for ESC key
        {
            break;
        }
        if (c == 'x' || c == 'X')   //x stands for exit, will exit the code
        {
            break;
        }
        if (c == 's' || c == 'S') //if s key is hit, saves the image to a local file
        {
            imwrite("new_image.jpg", image);    //saving the image to a local file
            break;
        }
        if (c == 'r' || c == 'R' || c == 0)   //resets the image back to normal
        {
            resetImage();   //calls a method to reset the image
        }
        if (c == 'i' || c == 'I')  //make the blur blocks bigger
        {
            if (blur_degree > 5)
            {
                blur_degree += 5;  //increasing blur degree
            }
            else if (blur_degree > 1)
            {
                blur_degree++;
            }

            //apply blur to the selected region
            if (pt.x != -1 && pt.y != -1)
            {
                blurBlock(pt, selectionEnd);
                image.copyTo(image_org);
            }
        }

        if (c == 'd' || c == 'D')  //decrease the block size
        {
            if (blur_degree < 5)
            {
                blur_degree--;  //decreasing the blur block size
            }
            else
            {
                blur_degree -= 5;  
            }

            //apply blur to the selected region
            if (pt.x != -1 && pt.y != -1)
            {
                blurBlock(pt, selectionEnd);
                image.copyTo(image_org);
            }
        }
        

    }
    return 1;
}