TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib \
-lopencv_core \
-lopencv_imgcodecs \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_ml \
-lopencv_video \
-lopencv_videoio \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_flann \
-lopencv_viz \
-lopencv_sfm

# --- ENDOF --- OpenCV paths for headers and libs
