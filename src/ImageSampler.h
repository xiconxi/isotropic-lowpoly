//
// Created by pupa on 12/17/20.
//

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <random>

#include <Eigen/Core>

using Point2d = Eigen::RowVector2d;
using Point2i = Eigen::RowVector2i;

typedef Eigen::Matrix<double, -1, 2, Eigen::RowMajorBit> MatrixX2rd;

double inline area(double x1, double y1, double x2, double y2, double x3, double y3) {
    return -( (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3))/2.0;
}

double inline area(const Point2d& p1, const Point2d& p2, const Point2d& p3) {
    return area(p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y());
}

double inline clip(double x, double l, double r) {
    return (x > l)? (x < r ? x : r):l;
}

class TriRaster{
public:
    explicit TriRaster(const Point2i& p1, const Point2i& p2, const Point2i& p3): pts_(3, 2){
        pts_ << p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y();
        min_ << pts_.col(0).minCoeff(), pts_.col(1).minCoeff();
        max_ << pts_.col(0).maxCoeff(), pts_.col(1).maxCoeff();
        pixel = min_;
    }

    bool operator == (const TriRaster& rhs) const{ return pixel == rhs.pixel; }

    bool operator != (const TriRaster& rhs) const { return !operator==(rhs); }

    TriRaster& operator++() {
        while(has_next() && !in_triangle());
        return *this;
    }

    Point2i& operator * () { return pixel; }

    TriRaster& begin() {
        pixel = min_;
        if(in_triangle()) return *this;
        return this->operator++();
    }

    TriRaster& end() {
        pixel = max_;
        return *this;
    }

private:

    bool in_triangle() {
        double area1 = area(pts_(0, 0), pts_(0, 1), pts_(1, 0), pts_(1, 1), pixel[0], pixel[1]);
        double area2 = area(pts_(1, 0), pts_(1, 1), pts_(2, 0), pts_(2, 1), pixel[0], pixel[1]);
        double area3 = area(pts_(2, 0), pts_(2, 1), pts_(0, 0), pts_(0, 1), pixel[0], pixel[1]);
        return area1 >= 0 && area2 >= 0 && area3 >= 0;
    }

    bool has_next() {
        if(pixel.y() != max_.y()) {
            if(pixel.x() != max_.x())  pixel.x()++;
            else   pixel = Point2i (min_.x(), pixel.y()+1);
        } else  {
            if(pixel.x() == max_.x()) return false;
            else pixel.x()++;
        }
        return true;
    }

    Eigen::MatrixX2d pts_;
    Point2i min_, max_, pixel;
};


class TriOverRaster{
public:
    explicit TriOverRaster(const Point2d& p1, const Point2d& p2, const Point2d& p3): pts_(3, 2){
        pts_ << p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y();
        min_xy = pts_.colwise().minCoeff();
        Point2d dis = (pts_.colwise().maxCoeff() - min_xy).array().max(1e-5);
        dxy.x() = std::min(dis.x()/int(dis.x()), dis.x()/4);
        dxy.y() = std::min(dis.y()/int(dis.y()), dis.y()/4);
        max_ij.x() = std::round(dis.x()/dxy.x());
        max_ij.y() = std::round(dis.y()/dxy.y());
        ij = Point2i(0, 0);
    }

    bool operator == (const TriOverRaster& rhs) const{ return ij == rhs.ij; }

    bool operator != (const TriOverRaster& rhs) const { return !operator==(rhs); }

    TriOverRaster& operator++() {
        while(has_next() && !in_triangle());
        return *this;
    }

    Point2d operator * () { return  min_xy + Point2d(dxy.x()*ij.x(), dxy.y() * ij.y()); }

    TriOverRaster& begin() {
        ij = Point2i(0, 0);
        if(in_triangle()) return *this;
        return this->operator++();
    }

    TriOverRaster& end() {
        ij = max_ij;
        return *this;
    }

private:

    bool in_triangle() {
        Point2d pixel = this->operator*();
        double area1 = area(pts_(0, 0), pts_(0, 1), pts_(1, 0), pts_(1, 1), pixel[0], pixel[1]);
        double area2 = area(pts_(1, 0), pts_(1, 1), pts_(2, 0), pts_(2, 1), pixel[0], pixel[1]);
        double area3 = area(pts_(2, 0), pts_(2, 1), pts_(0, 0), pts_(0, 1), pixel[0], pixel[1]);
        return area1 >= 0 && area2 >= 0 && area3 >= 0;
    }

    bool has_next() {
        if(ij.y() != max_ij.y()) {
            if(ij.x() != max_ij.x())  ij.x()++;
            else   ij = Point2i (max_ij.x(), ij.y()+1);
        } else  {
            if(ij.x() == max_ij.x()) return false;
            else ij.x()++;
        }
        return true;
    }

    Eigen::MatrixX2d pts_;
    Point2i max_ij, ij;
    Point2d dxy, min_xy;
};

class ImageSampler: public Eigen::MatrixXd
{
public:
    explicit ImageSampler(const Eigen::MatrixXd& d): Eigen::MatrixXd(d){}

    double d(const Point2d& p) const  {
        double x = clip(p.x(), 0.0, cols()-1);
        double y = clip(p.y(), .0, rows()-1);
        size_t floor_x = std::floor(x), floor_y = std::floor(y);
        size_t ceil_x = std::ceil(x), ceil_y = std::ceil(y);
        double _x = x - floor_x, _y = y - floor_y;

        double floor_pixel = pixel_(floor_y, floor_x) * (1 - _x) + pixel_(ceil_y, floor_x) * _x;
        double ceil_pixel = pixel_(floor_y, ceil_x) * (1 - _x) + pixel_(ceil_y, ceil_x) * _x;
        return (floor_pixel * (1 - _y) + ceil_pixel * _y);
    }

    double d(const Point2i& p) const {
        return this->pixel_(clip(p.y(), 0, rows()-1), clip(p.x(), 0, cols()-1));
    }

    MatrixX2rd random_select(int n_sample) {
        n_sample = std::min(n_sample, int(size()/4) );
        if(n_sample == -1)
            n_sample = 128.0*128*rows()/cols();
        MatrixX2rd points_(n_sample, 2);

        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> dis(.0, 1.0);

        double d_scale = maxCoeff();
        for(size_t idx = 0; idx < n_sample; ){
            Point2d p(dis(gen) * (cols()-1), dis(gen) * (rows()-1));
            if(d( p ) < dis(gen)*d_scale) continue;
            points_.row(idx++) = p;
        }
        return points_;
    }

    MatrixX2rd dice_select(int n_sample) {
        n_sample = std::min(n_sample, int(size()/4) );
        if(n_sample == -1)
            n_sample = 128.0*128*rows()/cols();
        MatrixX2rd points_(n_sample, 2);

        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> dis(.0, 1.0);

        double d_scale = n_sample/sum();
        size_t idx = 0;
        for(int r = 0; r < rows(); r++) {
            for(int c = 0; c < cols(); c++) {
                if(idx >= points_.rows() || pixel_(r, c) * d_scale < dis(gen)  ) continue;
                points_.row(idx++) = Point2d(c, r);
            }
        }
        points_.conservativeResize(idx, 2);
        return points_;
    }

    Eigen::Vector4d weight_moment(const Point2d& p1, const Point2d& p2, const Point2d& p3) const {
        Eigen::Vector4d acc = Eigen::Vector4d::Zero(4); //(x*d, y*d, d, area)
        Point2d pc = (p1+p2+p3)/3.0;
        acc += Eigen::Vector4d(pc.x(), pc.y(), 1, 1) * d(pc);
        acc.w() = 1;
        for(Point2d p: TriOverRaster(p1, p2, p3) ) {
            acc.topRows(3) += Eigen::Vector3d(p.x(), p.y(), 1)* d(p);
            acc.w()++;
        }
        return acc ;
    }

    double geometric_median(const Point2d& p1, const Point2d& p2, const Point2d& p3) const  {
        std::vector<double> ds;
        for(int i = 0; i < 30; i++) {
            Eigen::RowVector3i v = Eigen::RowVector3i::Random().cwiseAbs();
            v[0] = v[0]%5+1; v[1] = v[1]%5+1; v[2] = v[2]%5+1;
            Point2d pc = (p1*v[0]+p2*v[1]+p3*v[2])/v.sum();
            ds.push_back(d(pc));
        }
        std::sort(ds.begin(), ds.end());
        return ds[ds.size()/2];
    }


    Eigen::RowVector4d bound_rect() const { return Eigen::RowVector4d(0, 0, cols()-1, rows()-1); }

private:
    double pixel_(size_t r, size_t c) const {
        return this->operator()(r, c);
    }
};

template<typename T, int Dim>
class EigenLet: public Eigen::Matrix<T, Dim, 1> {
public:
    using Data = Eigen::Matrix<T, Dim, 1>;
    using Eigen::Matrix<T, Dim, 1>::Matrix;
    virtual ~EigenLet() = default;

    EigenLet<T, Dim>& unique() {
        auto max_it = std::max_element(EigenLet::data(), EigenLet::data()+EigenLet::size());
        std::rotate(EigenLet::data(), max_it, EigenLet::data()+EigenLet::size());
        return *this;
    }

    bool operator<(const EigenLet<T, Dim>& other) const {
        for (size_t i=0; i<Dim; i++) {
            if ((*this)[i] < other[i]) return true;
            else if ((*this)[i] > other[i]) return false;
        }
        return false;
    }

    int hash() const {
        static Eigen::RowVector4i p(73856093, 19349663, 83492791, 100663319);
        return p.leftCols(Dim).dot((*this));
    }
};

template<typename KeyType>
struct EigenLetHash {
    inline int operator() (const KeyType& key) const {
        return key.hash();
    }
};
