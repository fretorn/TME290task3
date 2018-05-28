/*
 * Copyright (C) 2018 Ola Benderius
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp> //Added this
#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"

int32_t main(int32_t argc, char **argv) {
  int32_t retCode{0};
  auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
  if ((0 == commandlineArguments.count("name")) || (0 == commandlineArguments.count("cid")) || (0 == commandlineArguments.count("traincnn"))) {
    std::cerr << argv[0] << " accesses video data using shared memory provided using the command line parameter --name=." << std::endl;
    std::cerr << "Usage:   " << argv[0] << " --cid=<OpenDaVINCI session> --name=<name for the associated shared memory> [--id=<sender stamp>] [--verbose]" << std::endl;
    std::cerr << "         --name:    name of the shared memory to use" << std::endl;
    std::cerr << "         --traincnn: set 1 or 0 for training the tiny dnn example and saving a net binary" << std::endl;
    std::cerr << "         --verbose: when set, a thumbnail of the image contained in the shared memory is sent" << std::endl;
    std::cerr << "Example: " << argv[0] << " --cid=111 --name=cam0 --traincnn=1" << std::endl;
    retCode = 1;
  } else {
    bool const VERBOSE{commandlineArguments.count("verbose") != 0};
    bool const TRAINCNN{std::stoi(commandlineArguments["cid"]) == 1};
    uint32_t const WIDTH{1280};
    uint32_t const HEIGHT{960};
    uint32_t const BPP{24};
    uint32_t const ID{(commandlineArguments["id"].size() != 0) ? static_cast<uint32_t>(std::stoi(commandlineArguments["id"])) : 0};

    std::string const NAME{(commandlineArguments["name"].size() != 0) ? commandlineArguments["name"] : "/cam0"};
    float const scalarAngle{(commandlineArguments["scalarAngle"].size() != 0) ? static_cast<float>(std::stoi(commandlineArguments["scalarAngle"])) : 0};
    float const constantAngle{(commandlineArguments["constantAngle"].size() != 0) ? static_cast<float>(std::stoi(commandlineArguments["constantAngle"])) : 0};
    float const scalarDistance{(commandlineArguments["scalarDistance"].size() != 0) ? static_cast<float>(std::stoi(commandlineArguments["scalarDistance"])) : 0};
    float const constantDistance{(commandlineArguments["constantDistance"].size() != 0) ? static_cast<float>(std::stoi(commandlineArguments["constantDistance"])) : 0};
    float const testVariables{(commandlineArguments["testVariables"].size() != 0) ? static_cast<float>(std::stoi(commandlineArguments["testVariables"])) : 0};
    int const lowH{(commandlineArguments["lowH"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["lowH"])) : 0};
    int const highH{(commandlineArguments["highH"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["highH"])) : 0};
    int const lowS{(commandlineArguments["lowS"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["lowS"])) : 0};
    int const highS{(commandlineArguments["highS"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["highS"])) : 0};
    int const lowV{(commandlineArguments["lowV"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["lowV"])) : 0};
    int const highV{(commandlineArguments["highV"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["highV"])) : 0};
    int const saveImage{(commandlineArguments["saveImage"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["saveImage"])) : 0};
    int const Hough_cannyEdgeThreshold{(commandlineArguments["Hough_cannyEdgeThreshold"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["Hough_cannyEdgeThreshold"])) : 0};
    int const Hough_centerThreshold{(commandlineArguments["Hough_centerThreshold"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["Hough_centerThreshold"])) : 0};
    int const Hough_minRadius{(commandlineArguments["Hough_minRadius"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["Hough_minRadius"])) : 0};
    int const Hough_maxRadius{(commandlineArguments["Hough_maxRadius"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["Hough_maxRadius"])) : 0};
    int const NAMEvariableIsVoid{(commandlineArguments["NAMEvariableIsVoid"].size() != 0) ? static_cast<int>(std::stoi(commandlineArguments["NAMEvariableIsVoid"])) : 0};

    cluon::OD4Session od4{static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))};

    std::unique_ptr<cluon::SharedMemory> sharedMemory(new cluon::SharedMemory{NAME});
    if (sharedMemory && sharedMemory->valid()) {
      std::clog << argv[0] << ": Found shared memory '" << sharedMemory->name() << "' (" << sharedMemory->size() << " bytes)." << std::endl;

    std::cerr << argv[0] << "hej" << std::endl;

    if (VERBOSE) {
      std::cerr << argv[0] << "saveImage " << saveImage 
      << "Hough_cannyEdgeThreshold" << Hough_cannyEdgeThreshold
      << "Hough_centerThreshold" << Hough_centerThreshold
      << "Hough_minRadius" << Hough_minRadius
      << "Hough_maxRadius" << Hough_maxRadius
      << "NAMEvariableIsVoid" << NAMEvariableIsVoid
      << "testVariables" << testVariables
      << "lowH" << lowH
      << std::endl;
    }

    if (NAMEvariableIsVoid == 1) {
      (void)NAME;
    }
    
    (void)TRAINCNN;

    CvSize size;
    size.width = WIDTH;
    size.height = HEIGHT;

    IplImage *image = cvCreateImageHeader(size, IPL_DEPTH_8U, BPP/8);
    sharedMemory->lock();
    image->imageData = sharedMemory->data();
    image->imageDataOrigin = image->imageData;
    sharedMemory->unlock();

    float estimatedDetectionAngle = 0.0f;
    float estimatedDetectionDistance = 0.0f;
    float radius = 0.0f;
    estimatedDetectionAngle = testVariables;
    estimatedDetectionDistance = testVariables;

    int32_t i = 0;
    while (od4.isRunning()) {
      sharedMemory->wait();

      // Make a scaled copy of the original image.
      int32_t const width = 256;
      int32_t const height = 196;
      cv::Mat scaledImage;
      cv::Mat imgOriginal;
      cv::Mat hsvImg; //HSV image
      cv::Mat threshImg;  // Thresh Image

      {
        sharedMemory->lock();
        cv::Mat sourceImage = cv::cvarrToMat(image, false);
        cv::resize(sourceImage, scaledImage, cv::Size(width, height), 0, 0, cv::INTER_NEAREST);
        sharedMemory->unlock();
      }

      cv::cvtColor(scaledImage, hsvImg, CV_BGR2HSV); //Convert image to HSV from BGR
      cv::inRange(hsvImg, cv::Scalar(lowH, lowS, lowV), cv::Scalar(highH, highS, highV), threshImg);
      cv::GaussianBlur(threshImg, threshImg, cv::Size(3, 3), 0);			//Blur Effect
      cv::dilate(threshImg, threshImg, 0);								// Dilate Filter Effect
      cv::erode(threshImg, threshImg, 0);									// Erode Filter Effect

      std::vector<cv::Vec3f> v3fCircles;		// 3 element vector of floats, this will be the pass by reference output of HoughCircles()

      // fill circles vector with all circles in processed image
      cv::HoughCircles(threshImg,v3fCircles,CV_HOUGH_GRADIENT,2,threshImg.rows / 4,
      Hough_cannyEdgeThreshold,Hough_centerThreshold,Hough_minRadius,Hough_maxRadius);  // algorithm for detecting circles		

      for (unsigned int j = 0; j < v3fCircles.size(); j++) {						// for each circle

        if (VERBOSE) {
          std::cerr << argv[0] << "Ball position X = "<< v3fCircles[j][0]			// x position of center point of circle
          <<",\tY = "<< v3fCircles[j][1]								// y position of center point of circle
          <<",\tRadius = "<< v3fCircles[j][2]<< "\n";					// radius of circle
        
                                
                                          // draw small green circle at center of object detected
        cv::circle(imgOriginal,												// draw on original image
          cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]),		// center point of circle
          3,																// radius of circle in pixels
          cv::Scalar(0, 255, 0),											// draw green
          CV_FILLED);														// thickness

                                          // draw red circle around object detected 
        cv::circle(imgOriginal,												// draw on original image
          cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]),		// center point of circle
          (int)v3fCircles[i][2],											// radius of circle in pixels
          cv::Scalar(0, 0, 255),											// draw red
          3);																// thickness

        }
        estimatedDetectionAngle = constantAngle + scalarAngle*v3fCircles[j][0]/100;
        estimatedDetectionDistance = constantDistance + scalarDistance/v3fCircles[j][2];
        radius = v3fCircles[j][2];
      }	

      // Make an estimation.
      
      if (VERBOSE) {

        if (saveImage == 1) { //scaledImage
          // std::cerr << argv[0] << "Saving scaledImage" << std::endl;
          std::string const FILENAME = std::to_string(i) + ".jpg";
          cv::imwrite(FILENAME, scaledImage);
          i++;
        } else if (saveImage == 2) { //threshImg
          // std::cerr << argv[0] << "Saving threshImg" << std::endl;
          std::string const FILENAME = std::to_string(i) + ".jpg";
          cv::imwrite(FILENAME, threshImg);
          i++;
        } else if (saveImage == 3) { //hsvImg
          // std::cerr << argv[0] << "Saving hsvImg" << std::endl;
          std::string const FILENAME = std::to_string(i) + ".jpg";
          cv::imwrite(FILENAME, hsvImg);
          i++;
        } else if (saveImage == 4) { //With circlesImage
          // std::cerr << argv[0] << "Saving circlesImage" << std::endl;
          std::string const FILENAME = std::to_string(i) + ".jpg";
          cv::imwrite(FILENAME, imgOriginal);
          i++;
        } else if (saveImage == 5) {
          // std::cerr << argv[0] << "Not saving any image" << std::endl;
        }

        // std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cerr << argv[0] << "Target angle " << std::setw(6) << estimatedDetectionAngle 
          << " distance " << std::setw(6) << estimatedDetectionDistance
          << " radius " << std::setw(6) << radius
          << std::endl;
      }

      // In the end, send a message that is received by the control logic.
      opendlv::logic::sensation::Point detection;
      detection.azimuthAngle(estimatedDetectionAngle);
      detection.distance(estimatedDetectionDistance);

      od4.send(detection, cluon::time::now(), ID);
    }

      cvReleaseImageHeader(&image);
    } else {
      std::cerr << argv[0] << ": Failed to access shared memory '" << NAME << "'." << std::endl;
    }
  }
  return retCode;
}

