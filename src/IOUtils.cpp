//
// Created by pupa on 2021/5/14.
//

#include "IOUtils.h"



#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stdint.h>

#include <iostream>
#include <fstream>


bool read_density(std::string filename, Eigen::MatrixXd& gray) {
    int width_, height_, n_;
    if (filename.empty()) return false;
    std::uint8_t* img = stbi_load(filename.c_str(), &width_, &height_, &n_, 1);
    if ( img == nullptr) return false;

    gray.resize(height_, width_);
    for (int j = 0; j < height_; j++)
        for (int i = 0; i < width_; i++)
            gray(j, i) = img[(j * width_ + i)];

    stbi_image_free(img);
    return true;
}

bool read_density(std::string filename, Eigen::MatrixXd& R, Eigen::MatrixXd& G, Eigen::MatrixXd& B ) {
    int width_, height_, n_;
    if (filename.empty()) return false;
    std::uint8_t* img = stbi_load(filename.c_str(), &width_, &height_, &n_, 3);
    if ( img == nullptr) return false;

    R.resize(height_, width_);
    G.resize(height_, width_);
    B.resize(height_, width_);
    for (int j = 0; j < height_; j++)
        for (int i = 0; i < width_; i++) {
                R(j, i) = img[(j * width_ + i)*3+0];
                G(j, i) = img[(j * width_ + i)*3+1];
                B(j, i) = img[(j * width_ + i)*3+2];
        }

    stbi_image_free(img);
    return true;
}


void export_colored_off(std::string file_name, const MatrixX2rd& V, const Eigen::MatrixX3i& F,
                        const ImageSampler& R, const ImageSampler& G, const ImageSampler& B){
    Eigen::MatrixX3i color(F.rows(), 3);
    for(int fi = 0; fi < F.rows(); fi++) {
        auto p1 = V.row(F(fi, 0));
        auto p2 = V.row(F(fi, 1));
        auto p3 = V.row(F(fi, 2));
        auto r = R.geometric_median(p1, p2, p3);
        auto g = G.geometric_median(p1, p2, p3);
        auto b = B.geometric_median(p1, p2, p3);
//        color.row(fi) = Eigen::RowVector3i(r.z()/r.w(), g.z()/g.w(), b.z()/b.w());
        color.row(fi) = Eigen::RowVector3i( r, g, b );
    }

    std::ofstream svg_file(file_name+".off");
    svg_file << "OFF\n" << V.rows() << ' ' << F.rows() << ' ' << "0\n";
    double max_y = V.col(1).maxCoeff();
    for(size_t i = 0; i < V.rows(); i++) {
        svg_file << V(i, 0) << ' ' << max_y - V(i, 1) << " 0 \n";
    }

    for(size_t i = 0; i < F.rows(); i++ ) {
        svg_file << "3 " << F(i, 0) << ' ' << F(i, 1)<< ' ' << F(i, 2)
                 << ' ' << color.row(i) << std::endl;
    }

    svg_file.close();
}

void export_textured_obj(std::string file_name, const MatrixX2rd& V, const Eigen::MatrixX3i& F) {
    std::ofstream svg_file(file_name+".obj");
    double max_y = V.col(1).maxCoeff();
    double max_x = V.col(0).maxCoeff();
    for(size_t i = 0; i < V.rows(); i++) {
        svg_file << "v " << V(i, 0) << ' ' << max_y - V(i, 1) << " 0 \n";
    }
    for(size_t i = 0; i < V.rows(); i++) {
        svg_file << "vt " << V(i, 0)/max_x << ' ' << 1 - V(i, 1)/max_y << "\n";
    }

    auto F2 = F.array() + 1;
    for(size_t i = 0; i < F.rows(); i++ ) {
        svg_file << "f " << F2(i, 0) << '/' << F2(i, 0) << ' '
                 << F2(i, 1) << '/' << F2(i, 1) << ' '
                 << F2(i, 2) << '/' << F2(i, 2) << ' '<< std::endl;
    }

    svg_file.close();
}

void export_colored_obj(std::string file_name, const MatrixX2rd& V, const Eigen::MatrixX3i& F,
                        const ImageSampler& R, const ImageSampler& G, const ImageSampler& B) {
    std::ofstream svg_file(file_name+".obj");
    double max_y = V.col(1).maxCoeff();
    double max_x = V.col(0).maxCoeff();
    for(size_t i = 0; i < V.rows(); i++) {
        Point2d p = V.row(i);
        svg_file << "v " << V(i, 0) << ' ' << max_y - V(i, 1) << " 0 "
        << int(R.d(p)) << ' ' << int(G.d(p)) << ' ' << int(B.d(p)) << "\n";
    }
    for(size_t i = 0; i < V.rows(); i++) {
        svg_file << "vt " << V(i, 0)/max_x << ' ' << 1 - V(i, 1)/max_y << "\n";
    }

    auto F2 = F.array() + 1;
    for(size_t i = 0; i < F.rows(); i++ ) {
        svg_file << "f " << F2(i, 0) << '/' << F2(i, 0) << ' '
                 << F2(i, 1) << '/' << F2(i, 1) << ' '
                 << F2(i, 2) << '/' << F2(i, 2) << ' '<< std::endl;
    }

    svg_file.close();
}



void export_svg(const MatrixX2rd& points, std::string output_file) {
    std::stringstream svg_str;
    Eigen::RowVector2d _max = points.colwise().maxCoeff();
    svg_str << "<svg width=\"" << _max[0] << "\" height=\"" << _max[1] << "\" viewBox=\"0 0 " << _max[0] << ' ' << _max[1]
            << R"(" xmlns="http://www.w3.org/2000/svg" >)"
            << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    for( int i = 0; i < points.rows(); ++i ) {
        svg_str << "<circle cx=\"" << points(i, 0) << "\" cy=\"" << points(i, 1) << R"(" r="1" fill="black"/>)" << '\n';
    }

    svg_str << "</svg>";

    std::ofstream svg_file(output_file);
    svg_file << svg_str.str();
    svg_file.close();
}

void export_colored_svg(std::string file_name, const MatrixX2rd& V, const Eigen::MatrixX3i& F,
                        const ImageSampler& R, const ImageSampler& G, const ImageSampler& B) {
    Eigen::MatrixX3i color(F.rows(), 3);
    for(int fi = 0; fi < F.rows(); fi++) {
        auto p1 = V.row(F(fi, 0));
        auto p2 = V.row(F(fi, 1));
        auto p3 = V.row(F(fi, 2));
        auto r = R.geometric_median(p1, p2, p3);
        auto g = G.geometric_median(p1, p2, p3);
        auto b = B.geometric_median(p1, p2, p3);
        color.row(fi) = Eigen::RowVector3i( r, g, b );
    }

    std::stringstream svg_str;
    Eigen::RowVector2d _max = V.colwise().maxCoeff();
    svg_str << "<svg width=\"" << _max[0] << "\" height=\"" << _max[1] << "\" viewBox=\"0 0 " << _max[0] << ' ' << _max[1]
            << R"(" xmlns="http://www.w3.org/2000/svg" >)"
            << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    for( int i = 0; i < F.rows(); ++i ) {
        auto p1 = V.row(F(i, 0));
        auto p2 = V.row(F(i, 1));
        auto p3 = V.row(F(i, 2));
        svg_str << "<polygon points=\"" << p1.x() << ',' << p1.y() << ' '
                << p2.x() << ',' << p2.y() << ' '
                << p3.x() << ',' << p3.y() << "\" style=\" stroke-width:0; stroke: none; fill:rgb("
                << color(i, 0) << ',' << color(i, 1) << ',' << color(i, 2) << ")\" />\n";
    }

    svg_str << "</svg>";

    std::ofstream svg_file(file_name);
    svg_file << svg_str.str();
    svg_file.close();
}