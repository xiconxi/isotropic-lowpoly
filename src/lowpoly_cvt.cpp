#include <array>
#include <string>
#include <sstream>
#include <iostream>

#include "ImageSampler.h"
#include "JCVoronoi.h"
#include "IOUtils.h"


int main(int argc, char** argv) {
    if(argc != 6) {
        assert(argc == 6 && "Usage: ./lowpoly_cvt ../data/gradient/1.bmp ../data/1.jpeg ../imgs/1 5000 15 " );
    }
    Eigen::MatrixXd gradient;
    assert( read_density(argv[1], gradient) );
    size_t n_sample = std::stoi(argv[4]);
    size_t n_iter = std::stoi(argv[5]);
    double edge_ratio = double( gradient.count() ) / gradient.size() ;
    ImageSampler sampler1(gradient);
    sampler1.array() = sampler1.array().max(1).min(1).eval();
    std::cout << gradient.count() << ' ' << gradient.size() << std::endl;
    MatrixX2rd Pts1 = sampler1.dice_select(n_sample*(1-edge_ratio));
    for(int i = 0; i < 5; i++) {
        double abs_changed =  cvt_lloyd_relaxation(Pts1, sampler1);
        std::cout << "uniform: " << i << ' ' << abs_changed << std::endl;
//        export_svg(Pts1, std::to_string(i)+"uniform.svg");
    }

    ImageSampler sampler2(gradient);
    sampler2.array() = sampler2.array().pow(3);
    MatrixX2rd Pts2 = sampler2.dice_select(n_sample* edge_ratio );

    MatrixX2rd Pts(Pts1.rows()+Pts2.rows(), 2);
    Pts.topRows(Pts1.rows()) = Pts1;
    Pts.bottomRows(Pts2.rows()) = Pts2;
//    export_svg(Pts, "gradient.svg");


    sampler2.array() = sampler2.array().pow(0.5/3.0).max(0.1);
    for(int i = 0; i < n_iter; i++) {
        double abs_changed =  cvt_lloyd_relaxation(Pts, sampler2);
        std::cout << "lloyd: " << i << ' ' << abs_changed << std::endl;
//        export_svg(Pts, std::to_string(i)+"lloyd.svg");
    }

    MatrixX2rd V2;
    Eigen::MatrixX3i F2;
    delaunay_triangulation(Pts, V2, F2);
    Eigen::MatrixXd rgb[3];
    read_density(argv[2], rgb[0], rgb[1], rgb[2]);
    ImageSampler R(rgb[0]), G(rgb[1]), B(rgb[2]);

    export_colored_svg(std::string(argv[3])+"lowpoly_tri.svg", V2, F2, R, G, B);
    export_svg(Pts, std::string(argv[3])+"lowpoly_point.svg");
//    export_colored_off("cvt.off", V2, F2, R, G, B );
//
//    export_colored_obj("cvt_approximate", V2, F2, R, G, B);
//    export_textured_obj("cvt", V2, F2);
}
