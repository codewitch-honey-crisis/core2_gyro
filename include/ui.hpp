#pragma once
#include <gfx.hpp>
#include <uix.hpp>
#include <cube3d.hpp>
// colors for the UI
using color_t = gfx::color<gfx::rgb_pixel<16>>; // native
using color32_t = gfx::color<gfx::rgba_pixel<32>>; // uix

// the screen template instantiation aliases
using screen_t = uix::screen<gfx::rgb_pixel<16>>;
using surface_t = screen_t::control_surface_type;

using label_t = uix::label<surface_t>;

template<typename ControlSurfaceType>
class gyro_box : public uix::control<ControlSurfaceType> {
   public:
    using control_surface_type = ControlSurfaceType;
    using base_type = uix::control<control_surface_type>;
    using pixel_type = typename base_type::pixel_type;
    using palette_type = typename base_type::palette_type;
    using bitmap_type = gfx::bitmap<pixel_type, palette_type>;

   private:
    cube3d<ControlSurfaceType> m_cube;
   public:
    gyro_box(uix::invalidation_tracker &parent, const palette_type *palette = nullptr)
        : base_type(parent, palette) {
    }
    gyro_box(gyro_box &&rhs) {
        m_cube = rhs.m_cube;
        do_move_control(rhs);
    }
    
    gyro_box &operator=(gyro_box &&rhs) {
        m_cube = rhs.m_cube;
        do_move_control(rhs);
        return *this;
    }
    gyro_box(const gyro_box &rhs) {
        m_cube = rhs.m_cube;
        do_copy_control(rhs);
    }
    gyro_box &operator=(const gyro_box &rhs) {
        m_cube = rhs.m_cube;
        do_copy_control(rhs);
        return *this;
    }
    virtual ~gyro_box() {
       
    }
    void set(gfx::spoint16 center, float scale, float x, float y,float z) {
        m_cube.set(center,scale,x,y,z);
        m_cube.update();
        this->invalidate();
    }
    
    virtual bool on_touch(size_t locations_size, const gfx::spoint16 *locations) {
        return true;
    }
    virtual void on_release() override {
        
    }
    virtual void on_paint(control_surface_type &destination, const gfx::srect16 &clip) override {
        m_cube.draw(destination,color_t::white);
    }
};
using gyro_box_t = gyro_box<surface_t>;

// the screen/control declarations
extern screen_t main_screen;
extern label_t main_title;
extern gyro_box_t main_cube;