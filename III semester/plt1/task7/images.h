#ifndef IMAGES_H
#define IMAGES_H

#include <functional>
#include <cmath>

#include "color.h"
#include "coordinate.h"
#include "functional.h"

/*********************** Types definitions ***********************/

typedef double Fraction;

template<typename T>
using Base_image = std::function<T(const Point)>;

using Region = Base_image<bool>;
using Image = Base_image<Color>;
using Blend = Base_image<Fraction>;

/*********************** Function templates definitions ***********************/

template<typename T>
Base_image<T> constant(T t) {
    return [=](const Point &) {
        return t;
    };
}

template<typename T>
Base_image<T> rotate(Base_image<T> image, double phi) {
    return compose([=](const Point p) {
        Point temp = p.is_polar ? p : to_polar(p);
        Point rotated_p(temp.first, temp.second - phi, true);
        return image(rotated_p);
    });
}

template<typename T>
Base_image<T> translate(Base_image<T> image, Vector v) {
    return compose([=](const Point p) {
        Point temp = p.is_polar ? from_polar(p) : p;
        Point translated_p(temp.first - v.first, temp.second - v.second);
        return image(translated_p);
    });
}

template<typename T>
Base_image<T> scale(Base_image<T> image, double s) {
    return compose([=](const Point p) {
        Point temp = p.is_polar ? from_polar(p) : p;
        Point scaled_p(temp.first / s, temp.second / s);
        return image(scaled_p);
    });
}

template<typename T>
Base_image<T> circle(Point q, double r, T inner, T outer) {
    return compose([=](const Point p) {
        Point temp = p.is_polar ? from_polar(p) : p;
        Point temp_q = q.is_polar ? from_polar(q) : q;
        return distance(temp, temp_q) <= r ? inner : outer;
    });
}

template<typename T>
Base_image<T> checker(double d, T this_way, T that_way) {
    return compose([=](const Point p) {
        int x = static_cast<int>(floor(p.first / d));
        int y = static_cast<int>(floor(p.second / d));
        return (x + y) % 2 == 0 ? this_way : that_way;
    });
}

template<typename T>
Base_image<T> polar_checker(double d, int n, T this_way, T that_way) {
    return compose([=](const Point p) {
        Point temp = p.is_polar ? p : to_polar(p);
        double new_phi = fmod(temp.second, 2 * M_PI);
        new_phi >= 0 ? new_phi : new_phi += 2 * M_PI;
        Point polar_point = {temp.first, (floor(new_phi / (2 * M_PI / n)) + d / n) * d, true};
        return checker(d, this_way, that_way)(polar_point);
    });
}

template<typename T>
Base_image<T> rings(Point q, double d, T this_way, T that_way) {
    return compose([=](const Point p) {
        Point temp = p.is_polar ? from_polar(p) : p;
        Point temp_q = q.is_polar ? from_polar(q) : q;
        return fmod(distance(temp, temp_q), 2 * d) <= d ? this_way : that_way;
    });
}

template<typename T>
Base_image<T> vertical_stripe(double d, T this_way, T that_way) {
    return compose([=](const Point p) {
        Point temp = p.is_polar ? from_polar(p) : p;
        return fabs(temp.first) < d / 2 ? this_way : that_way;
    });
}

/*********************** Function definitions ***********************/

inline Image cond(const Region &region, const Image &this_way, const Image &that_way) {
    return compose([=](Point p) {
        return region(p) ? this_way(p) : that_way(p);
    });
}

inline Image lerp(const Blend &blend, const Image &this_way, const Image &that_way) {
    return compose([=](Point p) {
        return this_way(p).weighted_mean(that_way(p), blend(p));
    });
}

inline Image darken(const Image &image, const Blend &blend) {
    return lerp(blend, image, constant<Color>(Colors::black));
}

inline Image lighten(const Image &image, const Blend &blend) {
    return lerp(blend, image, constant<Color>(Colors::white));
}

#endif // IMAGES_H
