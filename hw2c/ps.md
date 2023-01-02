# Assignment 2c: Interactive 3D viewing and projection transformations

This assignment focuses on the implementation of 3D viewing and projection transformations.  In this assignment you are asked to begin with a provided template (TemplateHW2c.zip) and:

1. Define a suitable initial viewing matrix for use in an interactive first-person style walkthrough within the provided 3D building model. To do this, you will need to define:
    1. a suitable initial location for the camera
    1. an initial direction of view (or the location of a lookat point, from which the view direction can be inferred)
    1. (c) an 'up' direction that defines the roll of the camera about the view direction.
1. The initial viewing position can be anywhere you prefer, but the configuration should attempt to approximate a first-person view from a standing position within the building. The initial values may be hard-coded into your program.
1. Allow the user to iteratively re-define the position and orientation of the camera/viewpoint with respect to the surrounding scene by using the arrow keys on the keyboard. Pressing the 'up' arrow should move the camera forward in the direction of view; pressing the 'down' arrow should move the camera backwards; pressing the 'left' arrow should rotate the viewing direction to the left (counterclockwise) about the vertical axis through the current camera position; pressing the 'right' arrow should rotate the viewing direction the other way.
1. Define an appropriate perspective transformation matrix to produce a realistic 2D view of the 3D scene. Note that you will need to ensure that the aspect ratio of the viewing frustum matches the aspect ratio of the window within which the view is being displayed.  The perspective transformation matrix can either be defined by the distance from the eye to the near clipping plane along with the distances from the center of the near clipping plane to the left, right, bottom, top, near and far boundaries of the viewing frustum, or by the specification of an aspect ratio and one field of view angle (either horizontal or vertical).  Appropriate initial values may be hard-coded into your program.  Please be sure to define your matrix so that only a reasonable (and not unreasonably large) amount of perspective distortion is evident in the image.  For instance, if you place the near clipping plane close to the eye, you will want to use small values for the left, right, bottom, and top distances to ensure that the horizontal and vertical field of view angles are not excessively large.
1. Allow the user to dynamically re-size the window while preserving an undistorted view of the scene. To do this, you will need to concurrently adjust both the dimensions of the viewport and the aspect ratio of the viewing frustum to match the new window dimensions.  Please note that on retina displays, the required number of pixels for the viewport command may be 2x the reported size of the window.  You can use the command glfwGetFramebufferSize(window, &width, &height); in your window resize callback to check for this situation.

Please turn in:

- all of your source code
- a series of images or short video (preferred) showing your program in action
- any special instructions that the TA will need to compile and run your code

