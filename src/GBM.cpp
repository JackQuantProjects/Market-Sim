#include <vector>
#include <random>
#include <cmath>

class GBM{
    private:
    std::vector<float> path;
    public:
    GBM(){}

    std::vector<float> BrownianMotion(float vol, float initial_price) {
        std::vector<float> path(252 + 1);

        float dt = 1 / 252;
        float drift = 0.05f;

        std::random_device rd;
        std::mt19937 generator(rd());
        std::normal_distribution<float> normal(0.0f, 1.0f);

        path[0] = initial_price;

        for (int i = 1; i <= 252; i++) {
        float Z = normal(generator);

        path[i] = path[i - 1] * std::exp((drift - 0.5f * vol * vol) * dt +
                                        vol * std::sqrt(dt) * Z);
        }

        return path;
    }
};