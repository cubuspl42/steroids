#ifndef STEROIDS_VEC2_H
#define STEROIDS_VEC2_H


#include "json.hpp"

struct vec2 {
    int x = 0;
    int y = 0;

    vec2(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator==(vec2 o);

    bool operator!=(vec2 o);

    vec2 operator+(vec2 o);

    nlohmann::json toJson() const;

    static vec2 fromJson(nlohmann::json j);
};


#endif //STEROIDS_VEC2_H
