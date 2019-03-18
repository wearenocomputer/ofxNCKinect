# ofxncKinect

KINECT IS ALIVE! After we heard the news Microsoft is reviving the Kinect we decided to open upload our openFrameworks KinectV2 library.

This code is a collection of stuff we have been using for the last few years.
Speed comes first, so we tried to create code that let's us choose what features we would need for a project (color vs no color, only 2d vs 3d tracking, etc...)
It includes stuff like user management, 2d and 3d scene management, sending data over the network, pointcloud triangulation, gesture detection, area management, etc...

It is no way an official API, so use it at your own risk. 

## Installation
This code only works on Windows 10/64 bit systems

1. Install openFrameworks:

*[OF 0.10.1](https://openframeworks.cc/download/) use the master branch of this repo
*[OF 0.9.8](https://openframeworks.cc/download/older/) use the of_0_9_8 branch of this repo

2. Install the KinectV2 sdk
https://www.microsoft.com/en-us/download/details.aspx?id=44561

To compile the examples use the openFrameworks project generator and the Kinect64.props file to set the paths to the Kinect SDK.  Build for 64 bit.

## Standing on the shoulders of giants
Love to the [openFrameworks](https://openframeworks.cc/) community!

In this repo we have used code from:
- [Roy Macdonald - ofxInfiniteCanvas](https://github.com/roymacdonald/ofxInfiniteCanvas)
- [Zach Lieberman- ofxMeshUtils](https://github.com/ofZach/ofxMeshUtils)
- [Vanderlin - ofxTimer](https://github.com/vanderlin/ofxTimer)
- [Rene Nyffenegger- Socket](https://github.com/ReneNyffenegger/Socket.cpp)

# EXAMPLES

## example
![example](https://raw.githubusercontent.com/wearenocomputer/ofxncKinect/master/images/2.jpg)
This is a basic example combining color, 2d and 3d Kinect data.
 
## exampleGestures

![example](https://raw.githubusercontent.com/wearenocomputer/ofxncKinect/master/images/4.gif)
Using custom trained gestures.
This example loads in a gbd file, in this case trained for the famous Crane Karate Kid Kick, and shows how to handle incoming data.

When using gestures make sure:
1. to #define GESTURES in the file 'ncKinectv2Core.h'
2. to copy the Kinect20.VisualGestureBuilder.dll file in the bin folder. See lib folder

 
## exampleTCPSender - exampleTCPReceiver

![example](https://raw.githubusercontent.com/wearenocomputer/ofxncKinect/master/images/3.jpg)
You can use multiple senders to send Kinect data over the network to the Kinect Receiver.
We support sending:
- pointcloud
- floorplane
- skeleton data

## exampleTriangulator
![example](https://raw.githubusercontent.com/wearenocomputer/ofxncKinect/master/images/1.jpg)
Example on how to create a mesh from user point cloud data in real time.

