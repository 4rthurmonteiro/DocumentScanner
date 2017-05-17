#include "mainwindow.h"

bool comparatorY  (Point pt1, Point pt2){
    return (pt1.y < pt2.y);
    }

bool comparatorX  (Point pt1, Point pt2){
    return (pt1.x < pt2.x);
    }

bool comparatorXY  (Point pt1, Point pt2){
    return (pt1.x <= pt2.x && pt1.y <= pt2.y);
    }

void filterPoints(vector<Point> &points) {

    vector<Point> points_temp;

    sort(points.begin(),points.end(),comparatorX);
    points_temp.push_back(points[0]);
    points_temp.push_back(points[points.size()-1]);

    sort(points.begin(),points.end(),comparatorY);
    points_temp.push_back(points[0]);
    points_temp.push_back(points[points.size()-1]);


    points.clear();
    points = points_temp;
    cout << "The filter points are "<<points<<endl;
}
