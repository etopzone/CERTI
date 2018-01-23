#ifndef BILLARD_BALL_H
#define BILLARD_BALL_H

#include <RTI/Handle.h>

#include <cmath>
#include <cstring>
#include <iostream>

#ifndef X_DISPLAY_MISSING
#include "../libgraphc/graph_c.hh"
#endif

struct Board {
    double width{500};
    double height{100};
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

    void setX(const double x)
    {
        my_x = x;
    }

    double getY() const
    {
        return my_y;
    }

    void setY(const double y)
    {
        my_y = y;
    }

    double getDX() const
    {
        return my_dx;
    }

    void setDX(const double dx)
    {
        my_dx = dx;
    }

    double getDY() const
    {
        return my_dy;
    }

    void setDY(const double dy)
    {
        my_dy = dy;
    }

    void updateX(const double x)
    {
        setDX(x - my_x);
        setX(x);
    }

    void updateY(const double y)
    {
        setDY(y - my_y);
        setY(y);
    }

    bool checkCollisionWith(const Ball& other)
    {
        return std::sqrt(std::pow(my_x + my_dx - other.getX(), 2) + std::pow(my_y + my_dy - other.getY(), 2))
            <= 2 * my_radius;
    }

    void bounceAgainst(const Ball& other)
    {
        if (std::memcmp(getHandle().encode().data(), other.getHandle().encode().data(), getHandle().encode().size())
            > 0) {
            return;
        }

        if ((my_dx == other.getDX()) && (my_dy == other.getDY())) {
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

    void display()
    {
#ifndef X_DISPLAY_MISSING
        cercler disque;
        point centre;

        centre.X = (int) my_x;
        centre.Y = (int) my_y;
        disque = Definecr(centre, (int) my_radius, COUL_UNIE, my_is_active ? (couleur) BLACK : GRAY);
        Drawcr(disque);
#else
        std::wcout << my_handle << " [" << my_x << ";" << my_y << "]\n";
#endif
    }

    void erase()
    {
#ifndef X_DISPLAY_MISSING
        cercler disque;
        point centre;

        centre.X = (int) my_x;
        centre.Y = (int) my_y;
        disque = Definecr(centre, (int) my_radius, COUL_UNIE, WHITE);
        Drawcr(disque);
#endif
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
