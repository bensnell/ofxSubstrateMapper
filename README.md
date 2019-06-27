# ofxSubstrateMapper
Map 3D points to the surface of the nearest substrate.

This addon allows you to define a substrate--a vertical surface on which imagery is mapped (either by using projectors directed toward a wall or a screen mounted to a wall, etc)--and map coordinates in physical space to the nearest point in screen space. Coordinates in screen space and distance/directionality of projection are provided for every mapped point.

The input file substrate.plan is defined in the header file provided, and an example is also supplied in bin>data.

This addon requires coordinates to be in the same space as the substrate. If they are not, use [ofxCalibrationTool](https://github.com/bensnell/ofxCalibrationTool) to get them there.
