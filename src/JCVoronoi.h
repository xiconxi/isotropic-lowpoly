//
// Created by pupa on 2021/5/14.
//

#pragma once

#define JCV_REAL_TYPE double
#include "jc_voronoi.h"
#include "ImageSampler.h"

void delaunay_triangulation(MatrixX2rd& points, MatrixX2rd& V2, Eigen::MatrixX3i& F2);

Eigen::MatrixX3i tri_tri_adjacency(const Eigen::MatrixX3i& F);

Point2d weighted_cell_centroid(const ImageSampler& density, const jcv_site* site) ;

double cvt_lloyd_relaxation(MatrixX2rd& points_, const ImageSampler& density);


