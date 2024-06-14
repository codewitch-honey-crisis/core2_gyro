#pragma once
#include <gfx.hpp>
#include <stddef.h>
#include <memory.h>
#include <math.h>
typedef struct {
    float x, y, z;
}  point3d_t;
typedef struct {
    size_t first;
    size_t second;
} edge3d_t;
using point2d_t = gfx::spoint16;
typedef struct {
    point3d_t cop, look_at, up;
    point2d_t center, screen;
    float focal;
    point3d_t basis_a,basis_c, look_dir;
} camera_t;
template<typename Destination>
class cube3d {
    static point3d_t points[];
    static constexpr const float pi = 3.141592654f; 
    static constexpr const size_t points_size = 8;
    point3d_t m_points[points_size];
    static constexpr const size_t edges_size = 12;
    static edge3d_t edges[edges_size];
    point2d_t m_points2d[points_size];
    camera_t m_camera;
    static void matrix_normalize(point3d_t& value) {
        // TODO: eliminate divs
        float length = sqrtf(value.x * value.x + value.y * value.y + value.z * value.z);
        value.x/=length;
        value.y/=length;
        value.z/=length;
    }
    static point3d_t matrix_cross_product(const point3d_t& lhs,const point3d_t& rhs) {
        return {
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x
        };
    }
    void rotate_x(float theta) {
        float th = (float)(pi * theta / 180);
        for (size_t i = 0; i < points_size; i++)
        {
            point3d_t& p = m_points[i];
            float x_ = p.x;
            float y_ = (float)(p.y * cosf(th) - p.z * sinf(th));
            float z_ = (float)(p.y * sinf(th) + p.z * cosf(th));

            p.x = x_;
            p.y = y_;
            p.z = z_;
        }
    }
    void rotate_y(float theta) {
        float th = (float)(pi * theta / 180);
        for (size_t i = 0; i < points_size; i++)
        {
            point3d_t& p = m_points[i];
            float x_ = (float)(p.z * sinf(th) + p.x * cosf(th));
            float y_ = p.y;
            float z_ = (float)(p.z * cosf(th) - p.x * sinf(th));

            p.x = x_;
            p.y = y_;
            p.z = z_;
        }
    }
    void rotate_z(float theta) {
        float th = (float)(pi * theta / 180);
        for (size_t i = 0; i < points_size; i++)
        {
            point3d_t& p = m_points[i];
            float x_ = (float)(p.x * cosf(th) - p.y * sinf(th));
            float y_ = (float)(p.x * sinf(th) + p.y * cosf(th));
            float z_ = p.z;

            p.x = x_;
            p.y = y_;
            p.z = z_;
        }
    }
    void translate_x(float tx) {
        for (size_t i = 0; i < points_size; i++)
        {
            m_points[i].x+=tx;
        }
    }
    void translate_y(float ty) {
        for (size_t i = 0; i < points_size; i++)
        {
            m_points[i].y+=ty;
        }
    }
    void translate_z(float tz) {
        for (size_t i = 0; i < points_size; i++)
        {
            m_points[i].z+=tz;
        }
    }
    void translate(float xr, float yr, float zr) {
        for (size_t i = 0; i < points_size; i++) {
            m_points[i].x+=xr;
            m_points[i].y+=yr;
            m_points[i].z+=zr;
        }
    }
    point3d_t transform_and_rotate(const point3d_t& a) const
    {
        point3d_t w = a;
        w.x -= m_camera.cop.x;
        w.y -= m_camera.cop.y;
        w.z -= m_camera.cop.z;
        return {
            w.x * m_camera.basis_a.x + w.y * m_camera.basis_a.y + w.z * m_camera.basis_a.z,
            w.x * m_camera.basis_c.x + w.y * m_camera.basis_c.y + w.z * m_camera.basis_c.z,
            w.x * m_camera.look_dir.x + w.y * m_camera.look_dir.y + w.z * m_camera.look_dir.z
        };
    }
    void rotate(point3d_t v1,point3d_t v2, float ang)
    {
        translate_x(v1.x * -1);
        translate_y(v1.y * -1);
        translate_z(v1.z * -1);
        
        float dx = v2.x - v1.x;
        float dy = v2.y - v1.y;
        float dz = v2.z - v1.z;

        float theta = (float)atan2f(dy, dx);
        float phi = (float)atan2f(sqrtf(dx * dx + dy * dy), dz);

        theta = (float)(theta * 180 / pi);
        phi = (float)(phi * 180 / pi);

        rotate_z(theta * -1);
        rotate_y(phi * -1);
        rotate_z(ang);

        rotate_y(phi * 1);
        rotate_z(theta * 1);

        translate_z(v1.z * 1);
        translate_y(v1.y * 1);
        translate_x(v1.x * 1);
    }
    point2d_t project(const point3d_t& w1)
    {
        point3d_t e1 = transform_and_rotate(w1);
        point3d_t n1 = {
            m_camera.focal * e1.x / e1.z,
            m_camera.focal * e1.y / e1.z
        };
        
        // view mapping
        point2d_t result;
        result.x = (int)(m_camera.center.x + n1.x);
        result.y = (int)(m_camera.center.y + n1.y);
        return result;
    }
    void camera_initialize() {
        m_camera.look_dir.x = m_camera.look_at.x - m_camera.cop.x;
        m_camera.look_dir.y = m_camera.look_at.y - m_camera.cop.y;
        m_camera.look_dir.z = m_camera.look_at.z - m_camera.cop.z;
        this->matrix_normalize(m_camera.look_dir);
        m_camera.basis_a = matrix_cross_product(m_camera.up, m_camera.look_dir);
        this->matrix_normalize(m_camera.basis_a);
        m_camera.basis_c = matrix_cross_product(m_camera.look_dir, m_camera.basis_a);
        this->matrix_normalize(m_camera.basis_c);
    }
    void copy_to(const cube3d& rhs) {
        memcpy(m_points,rhs.m_points,sizeof(m_points));
        memcpy(m_points2d,rhs.m_points2d,sizeof(m_points2d));
        m_camera = rhs.m_camera;
    }
    void scale(float scale) {
        for(int i = 0;i<points_size;++i) {
            m_points[i].x*=scale;
            m_points[i].y*=scale;
            m_points[i].z*=scale;
        }
    }
    static void dump_point(point2d_t p) {
        printf("(%d,%d)\n",(int)p.x,(int)p.y);
    }
    static void dump_point(const point3d_t& p) {
        printf("(%f,%f,%f)\n",p.x,p.y,p.z);
    }
public:
    cube3d() {
        m_camera.center = {160,120};
        m_camera.focal = 350;
        m_camera.cop = {0, 0, -450};
        m_camera.look_at = {0, 0, 50};
        m_camera.up = {0, 1, 0};
        this->camera_initialize();
    }
    cube3d(const cube3d& rhs) {
        copy_to(rhs);
    }
    cube3d& operator=(const cube3d& rhs) {
        copy_to(rhs);
        return *this;
    }
    cube3d(cube3d&& rhs) {
        copy_to(rhs);
    }
    cube3d& operator=(cube3d&& rhs) {
        copy_to(rhs);
        return *this;
    }
    void set(float scale, float x, float y, float z) {
        memcpy(m_points,points,points_size*sizeof(point3d_t));
        this->scale(scale);
        this->rotate_x(x);
        this->rotate_y(y);
        this->rotate_z(z);
    }
    void update() {
        for(int i = 0;i<points_size;++i) {
            m_points2d[i]=this->project(m_points[i]);
        }
    }
    void draw(Destination& destination, typename Destination::pixel_type color) {
        for(int i = 0;i<edges_size;++i) {
            gfx::draw::line_aa(destination,gfx::srect16(m_points2d[edges[i].first],m_points2d[edges[i].second]),color);
        }
    }
};
template<typename Destination>
point3d_t cube3d<Destination>::points[] = {
            {1,1,1},
            {1,1,-1},
            {-1,1,-1},
            {-1,1,1},
            {1,-1,1},
            {1,-1,-1},
            {-1,-1,-1},
            {-1,-1,1}
        };
template<typename Destination>
edge3d_t cube3d<Destination>::edges[] = {
        {0,1},
        {1,2},
        {2,3},
        {3,0},
        {4,5},
        {5,6},
        {6,7},
        {7,4},
        {0,4},
        {3,7},
        {2,6},
        {1,5}
    };