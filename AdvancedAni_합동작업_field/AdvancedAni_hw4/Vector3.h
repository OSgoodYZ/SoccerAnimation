////////////////////////////////////////////////////////////
//  Created by Yuki Koyama.                               //
//  Copyright (c) 2012 Yuki Koyama. All rights reserved.  //
//  http://www-ui.is.s.u-tokyo.ac.jp/~koyama/             //
//                                                        //
//  This is released for NON-COMMERCIAL use only.         //
////////////////////////////////////////////////////////////

#pragma once

#include <cmath>
#include <iostream>
#include <limits>

class Vector3d {
public:
    typedef double value_type;
    union {
        struct {
            value_type x_, y_, z_;
        };
        struct {
            value_type x, y, z;
        };
        value_type val[3];
    };
    
    // constructors ===============
    Vector3d() { fill(0); }
    ~Vector3d() {}
    
    Vector3d(value_type x, value_type y, value_type z) { set(x, y, z); }
    
    void set(value_type x, value_type y, value_type z) {
        x_ = x;
        y_ = y;
        z_ = z;
    }
    
    Vector3d& operator=(const Vector3d& src) {
        if (this != &src) {
            x_ = src.x_;
            y_ = src.y_;
            z_ = src.z_;
        }
        return *this;
    }
    
    bool operator==(const Vector3d& rhs) const { return equals(rhs); }
    bool operator!=(const Vector3d& rhs) const { return !equals(rhs); }
    bool equals(const Vector3d& rhs, double epsilon = std::numeric_limits<double>::epsilon()) const {
        double dx = x_ - rhs.x_;
        if (dx <= -epsilon || epsilon <= dx)
            return false;
        double dy = y_ - rhs.y_;
        if (dy <= -epsilon || epsilon <= dy)
            return false;
        double dz = z_ - rhs.z_;
        if (dz <= -epsilon || epsilon <= dz)
            return false;
        return true;
    }
    
    bool operator<(const Vector3d& rhs) const {
        if (x_ < rhs.x_) return true; else if (rhs.x_ < x_) return false;
        if (y_ < rhs.y_) return true; else if (rhs.y_ < y_) return false;
        if (z_ < rhs.z_) return true;
        return false;
    }
    bool operator<=(const Vector3d& rhs) const { return *this == rhs || *this < rhs; }
    bool operator>(const Vector3d& rhs) const { return rhs < *this; }
        bool operator>=(const Vector3d& rhs) const { return rhs <= *this; }
            
            void fill(const value_type& s) { x_ = y_ = z_ = s; }
            
            value_type& operator[](size_t index) { return (&x_)[index]; }
            const value_type& operator[](size_t index) const { return (&x_)[index]; }
            
            Vector3d& operator+=(value_type rhs) { x_ += rhs; y_ += rhs; z_ += rhs; return *this; }
            Vector3d& operator-=(value_type rhs) { x_ -= rhs; y_ -= rhs; z_ -= rhs; return *this; }
            Vector3d& operator*=(value_type rhs) { x_ *= rhs; y_ *= rhs; z_ *= rhs; return *this; }
            Vector3d& operator/=(value_type rhs) { x_ /= rhs; y_ /= rhs; z_ /= rhs; return *this; }
            Vector3d operator+(value_type rhs) const { return Vector3d(*this) += rhs; }
            Vector3d operator-(value_type rhs) const { return Vector3d(*this) -= rhs; }
            Vector3d operator*(value_type rhs) const { return Vector3d(*this) *= rhs; }
            Vector3d operator/(value_type rhs) const { return Vector3d(*this) /= rhs; }
            
            Vector3d& operator+=(const Vector3d& rhs) { x_ += rhs.x_; y_ += rhs.y_; z_ += rhs.z_; return *this; }
            Vector3d& operator-=(const Vector3d& rhs) { x_ -= rhs.x_; y_ -= rhs.y_; z_ -= rhs.z_; return *this; }
            Vector3d& operator*=(const Vector3d& rhs) { x_ *= rhs.x_; y_ *= rhs.y_; z_ *= rhs.z_; return *this; }
            Vector3d& operator/=(const Vector3d& rhs) { x_ /= rhs.x_; y_ /= rhs.y_; z_ /= rhs.z_; return *this; }
            Vector3d operator+(const Vector3d& rhs) const { return Vector3d(*this) += rhs; }
            Vector3d operator-(const Vector3d& rhs) const { return Vector3d(*this) -= rhs; }
            Vector3d operator*(const Vector3d& rhs) const { return Vector3d(*this) *= rhs; }
            Vector3d operator/(const Vector3d& rhs) const { return Vector3d(*this) /= rhs; }
            
            Vector3d operator-() const {
                return Vector3d(-x_, -y_, -z_);
            }
            
            value_type operator|(const Vector3d& rhs) const {
                return
                x_ * rhs.x_ +
                y_ * rhs.y_ +
                z_ * rhs.z_;
            }
            
            value_type dot_product(const Vector3d& rhs) const { return (*this) | (rhs); }
            Vector3d cross_product(const Vector3d& rhs) const {
                const Vector3d& lhs = *this;
                return Vector3d(
                                lhs[1] * rhs[2] - lhs[2] * rhs[1],
                                lhs[2] * rhs[0] - lhs[0] * rhs[2],
                                lhs[0] * rhs[1] - lhs[1] * rhs[0]);}
            // normalize
            value_type norm() const { return static_cast<value_type>(sqrt(sqnorm())); }
            value_type sqnorm() const { return (*this) | (*this); }
            value_type length() const { return norm(); }
            value_type lengthSquared() const { return sqnorm(); }
            Vector3d normalized() const { return (*this) / norm(); }
            Vector3d normalize() {
                (*this) /= norm();
                return *this;
            }
            void normalize_cond() {
                value_type r = norm();
                (*this) /= (r == static_cast<value_type>(0)) ? 1 : r;
            }
            };
            
            
