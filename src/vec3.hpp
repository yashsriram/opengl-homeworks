#ifndef VEC3_HPP
#define VEC3_HPP

class Vec3 {
public:
    float x, y, z;

    // this is to easily print a given vector to std for debugging
    friend std::ostream &operator<<(std::ostream &, const Vec3 &);

    Vec3() : x(0), y(0), z(0) {}

    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    bool operator==(const Vec3 &b) const {
        return this->x == b.x && this->y == b.y && this->z == b.z;
    }

    Vec3 operator+(const Vec3 &b) const {
        return {this->x + b.x, this->y + b.y, this->z + b.z};
    }

    Vec3 operator-(const Vec3 &b) const {
        return {this->x - b.x, this->y - b.y, this->z - b.z};
    }

    Vec3 operator*(float t) const {
        return {this->x * t, this->y * t, this->z * t};
    }

    float dot(const Vec3 &b) const {
        return this->x * b.x + this->y * b.y + this->z * b.z;
    }

    Vec3 cross(const Vec3 &b) const {
        return {this->y * b.z - this->z * b.y, this->z * b.x - this->x * b.z, this->x * b.y - this->y * b.x};
    }

    float abs() const {
        return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    }

    float absSquare() const {
        return this->x * this->x + this->y * this->y + this->z * this->z;
    }

    Vec3 unit() const {
        float abs = this->abs();
        if (abs < 1e-6) { return *this; }
        else { return *this * (1 / abs); }
    }

};

std::ostream &operator<<(std::ostream &out, const Vec3 &v) {
    out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return out;
}

#endif