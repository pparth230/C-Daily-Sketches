#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

// ASCII characters for different intensity levels (dark to bright)
const string ASCII_CHARS = " .:-=+*#%@";
const string ASMII_TEXT = "ASMII";

// Convert pixel intensity to ASCII character
char intensityToASCII(int intensity) {
  int index = (intensity * (ASCII_CHARS.length() - 1)) / 255;
  return ASCII_CHARS[index];
}

// Perform basic background subtraction segmentation
Mat performSegmentation(const Mat &frame) {
  Mat gray, blurred, edges, mask;

  // Convert to grayscale
  cvtColor(frame, gray, COLOR_BGR2GRAY);

  // Apply Gaussian blur
  GaussianBlur(gray, blurred, Size(5, 5), 0);

  // Edge detection
  Canny(blurred, edges, 50, 150);

  // Dilate edges to create mask
  dilate(edges, mask, Mat(), Point(-1, -1), 2);

  // Apply morphological operations
  morphologyEx(mask, mask, MORPH_CLOSE,
               getStructuringElement(MORPH_ELLIPSE, Size(15, 15)));

  // Find contours
  vector<vector<Point>> contours;
  findContours(mask.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

  // Create final mask with largest contour (assumed to be person)
  Mat finalMask = Mat::zeros(mask.size(), CV_8UC1);
  if (!contours.empty()) {
    // Find largest contour
    int largestIdx = 0;
    double largestArea = 0;
    for (int i = 0; i < contours.size(); i++) {
      double area = contourArea(contours[i]);
      if (area > largestArea) {
        largestArea = area;
        largestIdx = i;
      }
    }

    // Draw filled contour
    if (largestArea > 1000) { // Minimum area threshold
      drawContours(finalMask, contours, largestIdx, Scalar(255), FILLED);
    }
  }

  return finalMask;
}

// Convert edges to ASCII art with ASMII text pattern
Mat frameToASCII(const Mat &frame, const Mat &mask, int charWidth = 6,
                 int charHeight = 10) {
  Mat output = Mat::zeros(frame.size(), CV_8UC3);
  Mat edges;

  // Get edges from mask
  Canny(mask, edges, 50, 150);

  int asmiiIndex = 0;

  for (int y = 0; y < frame.rows; y += charHeight) {
    for (int x = 0; x < frame.cols; x += charWidth) {
      // Check if this region contains edges
      Rect roi(x, y, min(charWidth, frame.cols - x),
               min(charHeight, frame.rows - y));
      Mat edgeROI = edges(roi);

      // Calculate edge density in this region
      Scalar edgeMean = mean(edgeROI);

      // Only draw ASCII if edges are present
      if (edgeMean[0] > 30) {
        // Use ASMII text characters cyclically
        char asciiChar = ASMII_TEXT[asmiiIndex % ASMII_TEXT.length()];
        asmiiIndex++;

        // Bright color for edges
        Scalar color(100, 255, 255); // Cyan

        // Draw character
        putText(output, string(1, asciiChar), Point(x, y + charHeight - 2),
                FONT_HERSHEY_SIMPLEX, 0.4, color, 1);
      }
    }
  }

  return output;
}

int main() {
  // Open webcam
  VideoCapture cap(0);

  if (!cap.isOpened()) {
    cerr << "Error: Cannot open webcam" << endl;
    return -1;
  }

  cout << "Starting real-time segmentation with ASMII text..." << endl;
  cout << "Press 'q' to quit" << endl;

  Mat frame, mask, asciiFrame;

  while (true) {
    // Capture frame
    cap >> frame;

    if (frame.empty()) {
      cerr << "Error: Cannot read frame" << endl;
      break;
    }

    // Flip frame horizontally for mirror effect
    flip(frame, frame, 1);

    // Perform segmentation
    mask = performSegmentation(frame);

    // Convert to ASCII with ASMII text
    asciiFrame = frameToASCII(frame, mask);

    // Display results
    imshow("Original", frame);
    imshow("Segmentation Mask", mask);
    imshow("ASMII ASCII Segmentation", asciiFrame);

    // Check for quit
    if (waitKey(1) == 'q') {
      break;
    }
  }

  cap.release();
  destroyAllWindows();

  return 0;
}
