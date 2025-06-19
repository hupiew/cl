/**
 * This file is part of cl (Adapted from ORB-SLAM3)
 *
 * Copyright (C) 2025  Hupie (hupiew[at]gmail.com)
 * Copyright (C) 2017-2021 Carlos Campos, Richard Elvira, Juan J. Gómez Rodríguez,
 *                         José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
 * Copyright (C) 2014-2016 Raúl Mur-Artal, José M.M. Montiel and Juan D. Tardós,
 * University of Zaragoza.
 *
 * License info can be found at the end of this file.
 */

#pragma once

#include <QImage>
#include <extractor.h>
#include <opencv2/opencv.hpp>
#include <vector>


class ORBSLAM3 : public Extractor
{
public:
    enum
    {
        HARRIS_SCORE = 0,
        FAST_SCORE = 1
    };

    ORBSLAM3()
      : ORBSLAM3(500, 1.2, 8, 20, 7) // same as Soutubot/Aloxaf's imsearch
    {}

    ORBSLAM3(
        int nfeatures, float scaleFactor, int nlevels, int iniThFAST, int minThFAST);

    ~ORBSLAM3() { delete[] level_data; }

    virtual void extract(const QImage& image) override;

    virtual std::vector<int8_t> get_descriptor() const override { return { desc }; }

    virtual bool is_variable() const noexcept override { return false; }

    // Compute the ORB features and descriptors on an image.
    // ORB are dispersed on the image using an octree.
    // Mask is ignored in the current implementation.
    int operator()(cv::InputArray _image,
                   cv::InputArray _mask,
                   std::vector<cv::KeyPoint>& _keypoints,
                   cv::OutputArray _descriptors,
                   std::vector<int>& vLappingArea);

    std::vector<cv::Mat> mvImagePyramid;

protected:
    void ComputePyramid(cv::Mat image);
    void ComputeKeyPointsOctTree(std::vector<std::vector<cv::KeyPoint>>& allKeypoints);
    std::vector<cv::KeyPoint> DistributeOctTree(
        const std::vector<cv::KeyPoint>& vToDistributeKeys,
        const int& minX,
        const int& maxX,
        const int& minY,
        const int& maxY,
        const int& nFeatures);

    std::vector<cv::Point> pattern;

    int nfeatures;
    double scaleFactor;
    int nlevels;
    int iniThFAST;
    int minThFAST;

    std::vector<int> umax;

    float calc_scale_factor(int level) const;
    float get_scale_factor(int level) const;
    void set_scale_factor(int level, float value);

    float calc_inv_scale_factor(int level) const;
    float get_inv_scale_factor(int level) const;
    void set_inv_scale_factor(int level, float value);

    int calc_features(int level) const;
    int get_features(int level) const;
    void set_features(int level, int value);

    float* level_data = nullptr;
    std::vector<int8_t> desc;
};

/*
 * ORB-SLAM3 is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ORB-SLAM3 is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ORB-SLAM3. If not, see <http://www.gnu.org/licenses/>.
 */
