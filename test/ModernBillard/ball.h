#ifndef BILLARD_BALL_H
#define BILLARD_BALL_H

#include <RTI/Handle.h>

#include <cmath>

struct Board {
    double width{80};
    double height{24};
};

class Ball {
public:
    using handle = rti1516e::ObjectInstanceHandle;

    void init(const int seed)
    {
        my_x = my_radius + seed * 60 + 3;
        my_y = my_radius + seed * 20;

        if (seed % 2) {
            my_dx = -my_dx;
        }
    }

    bool isActive() const
    {
        return my_is_active;
    }

    void setActive(const bool state = true)
    {
        my_is_active = state;
    }

    void setInactive()
    {
        setActive(false);
    }

    handle getHandle() const
    {
        return my_handle;
    }

    void setHandle(const handle h)
    {
        my_handle = h;
    }

    double getX() const
    {
        return my_x;
    }

    double getY() const
    {
        return my_y;
    }

    double getDX() const
    {
        return my_dx;
    }

    double getDY() const
    {
        return my_dy;
    }

    bool checkCollisionWith(const Ball& other)
    {
        return std::sqrt(std::pow(my_x - my_dx - other.getX(), 2) + std::pow(my_y - my_dy - other.getY(), 2))
            <= 2 * my_radius;
    }

    void bounceAgainst(const Ball& other)
    {
        if ((my_dx == other.getX()) && (my_dy == other.getY())) {
            my_dx = -my_dx;
            my_dy = -my_dy;
        }
        else {
            my_dx = other.getDX();
            my_dy = other.getDY();
        }
    }

    void checkCollisionAndBounceWith(const Board& border)
    {
        if ((my_x < my_radius) || (my_x > border.width - my_radius)) {
            my_dx = -my_dx;
        }

        // top/bottom collision
        if ((my_y < my_radius) || (my_y > border.height - my_radius)) {
            my_dy = -my_dy;
        }
    }

    void moveWithCurrentInertia()
    {
        my_x += my_dx;
        my_y += my_dy;
    }

private:
    handle my_handle{};

    bool my_is_active{false};

    double my_x{-10.0};
    double my_y{-10.0};

    double my_radius{10.0};

    double my_dx{3.0};
    double my_dy{3.0};

//     double my_old_dx{0.0};
//     double my_old_dy{0.0};
};

#endif // BILLARD_BALL_H
