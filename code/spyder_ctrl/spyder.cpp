# include "spyder.hpp"

# define SPYDER_LEG_ANCHOR 50.0
# define SPYDER_LEG1_DEFAULT 50.0
# define SPYDER_LEG2_DEFAULT 80.0
# define SPYDER_LEG3_DEFAULT 50.0

int main() {
    Spyder spyder (
        Vector3f({ 0.0, SPYDER_LEG_ANCHOR, 0.0 }),
        { 
            std::array<Vector3f, 3>
            { Vector3f({ 0.0, SPYDER_LEG1_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG2_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG3_DEFAULT, 0.0 }) },
            { Vector3f({ 0.0, SPYDER_LEG1_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG2_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG3_DEFAULT, 0.0 }) },
            { Vector3f({ 0.0, SPYDER_LEG1_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG2_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG3_DEFAULT, 0.0 }) },
            { Vector3f({ 0.0, SPYDER_LEG1_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG2_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG3_DEFAULT, 0.0 }) },
            { Vector3f({ 0.0, SPYDER_LEG1_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG2_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG3_DEFAULT, 0.0 }) },
            { Vector3f({ 0.0, SPYDER_LEG1_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG2_DEFAULT, 0.0 }), Vector3f({ 0.0, SPYDER_LEG3_DEFAULT, 0.0 }) }
        }
    );

    spyder.goto_default_position();

    while (true) {
        
    }
}