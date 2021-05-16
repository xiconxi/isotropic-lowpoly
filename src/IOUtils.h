//
// Created by pupa on 2021/5/14.
//

#pragma once

#include <string>
#include <Eigen/Core>

#include "ImageSampler.h"

typedef Eigen::Matrix<double, -1, 2, Eigen::RowMajorBit> MatrixX2rd;

bool read_density(std::string filename, Eigen::MatrixXd& gray);

bool read_density(std::string filename, Eigen::MatrixXd& R, Eigen::MatrixXd& G, Eigen::MatrixXd& B );

void export_svg(const MatrixX2rd& points, std::string output_file);

void export_colored_svg(std::string file_name, const MatrixX2rd& V2, const Eigen::MatrixX3i& F2,
                        const ImageSampler& R, const ImageSampler& G, const ImageSampler& B);

void export_colored_off(std::string file_name, const MatrixX2rd& V2, const Eigen::MatrixX3i& F2,
                        const ImageSampler& R, const ImageSampler& G, const ImageSampler& B);


void export_colored_obj(std::string file_name, const MatrixX2rd& V2, const Eigen::MatrixX3i& F2,
                        const ImageSampler& R, const ImageSampler& G, const ImageSampler& B);

void export_textured_obj(std::string file_name, const MatrixX2rd& V2, const Eigen::MatrixX3i& F2);
