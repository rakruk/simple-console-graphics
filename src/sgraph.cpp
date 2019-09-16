#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <vector>

#define WIDTH		100
#define HEIGHT		25
#define EF_WIDTH	(WIDTH + 1)

class shape {};

template <typename T> class sqmat2 {
  public:
    std::array<T, 4> elem;
    template <typename... Types> sqmat2(Types... initlist) : elem{{initlist...}} {}
    void print() { printf("[ %f\t%f ]\n[ %f\t%f ]", elem[0], elem[1], elem[2], elem[3]); }
};

template <typename T> class lvect2 {
  public:
    std::array<T, 2> elem;
    template <typename... Types> lvect2(Types... initlist) : elem{{static_cast<T>(initlist)...}} {}
    void print() { printf("[ %f\t%f ]", elem[0], elem[1]); }

    // Addition
    friend lvect2 operator+(lvect2 vectA, lvect2<T> vectB) {
        return lvect2({vectA.elem[0] + vectB.elem[0], vectA.elem[1] + vectB.elem[1]});
    }
    // Substraction
    friend lvect2 operator-(lvect2 vectA, lvect2<T> vectB) {
        return lvect2({vectA.elem[0] - vectB.elem[0], vectA.elem[1] - vectB.elem[1]});
    }
    // Internal product (vector * square matrix)
    friend lvect2 operator*(lvect2 vect, sqmat2<T> mat) {
        return lvect2({vect.elem[0] * mat.elem[0] + vect.elem[1] * mat.elem[1],
                       vect.elem[0] * mat.elem[2] + vect.elem[1] * mat.elem[3]});
    }
    // Dot product
    friend T operator*(lvect2 vectA, lvect2<T> vectB) {
        return vectA.elem[0] * vectB.elem[0] + vectA.elem[1] * vectB.elem[1];
    }
    // Cross product
    friend T operator^(lvect2 vectA, lvect2<T> vectB) {
        return (vectA.elem[0] * vectB.elem[1]) - (vectA.elem[1] * vectB.elem[0]);
    }
};

template <typename T> class trig : public shape {
  public:
    // https://github.com/Wunkolo/qTriangle
    bool isIn(lvect2<T> vectpos) {
        return ((vertex[1] - vertex[0]) ^ (vectpos - vertex[0])) >= 0
               && ((vertex[2] - vertex[1]) ^ (vectpos - vertex[1])) >= 0
               && ((vertex[0] - vertex[2]) ^ (vectpos - vertex[2])) >= 0;
    }
    void transform(sqmat2<T> transmat) {
        for (auto &vect : vertex) {
            vect = vect * transmat;
        }
    }
    void translate(lvect2<T> addvect) {
        for (auto &vect : vertex) {
            vect = vect + addvect;
        }
    }
    std::array<lvect2<T>, 3> vertex;
    template <typename... Types> trig(Types... initlist) : vertex{{initlist...}} {}
};

void inline render() {
	
	// Frame declaration
    unsigned int tick = 0;
    char frame[HEIGHT * EF_WIDTH + 2] = {' '};

	// Static shapes declaration 
    trig<double> tri({lvect2<double>{10.0, -5.0}, lvect2<double>{0.0, 5.0}, lvect2<double>{-10.0, -5.0}});

	// Static transformation matrices translation vectors declaration 
    sqmat2<double> rotate({cos(0.02), -sin(0.02), sin(0.02), cos(0.02)});
    sqmat2<double> xstrech({2.0, 0.0, 0.0, 1.0});
	lvect2<double> recenter({WIDTH / 2.0, HEIGHT / 2.0});
    

    while (true) {
		// Dynamic transformation matrices
		sqmat2<double> dynamic_scale({sin(0.1 * tick) + 1.0, 0.0, 0.0, sin(0.1 * tick) + 1.0});
        tick++;
        
		// Transform operations
        tri.transform(rotate);

		// Volatile shape copy and transformations
        trig<double> recentertri(tri);
        recentertri.transform(dynamic_scale);
        recentertri.transform(xstrech);
        recentertri.translate(recenter);

        frame[0] = '\n';

        // Generate frame
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (recentertri.isIn(lvect2<double>{x, y})) {
                    frame[x + y * EF_WIDTH + 1] = '@';
                } else {
                    frame[x + y * EF_WIDTH + 1] = '-';
                }
            }
            frame[y * EF_WIDTH + WIDTH + 1] = '\n';
        }
        frame[HEIGHT * EF_WIDTH] = '\0';

        // Render
        puts(frame);
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

int main() { 
    render();
}