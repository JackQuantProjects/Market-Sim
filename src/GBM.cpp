#pragma once

#include <vector>
#include <random>
#include <cmath>

class GBM{
    public:
    GBM(){}

    // Steps over [0, duration] years, so dt matches the horizon the
    // quoting model is solving over.
    std::vector<float> BrownianMotion(float vol, float initial_price, float duration, int steps) {
        std::vector<float> path(steps + 1);

        float dt = duration / static_cast<float>(steps);
        float drift = 0.05f;

        std::random_device rd;
        std::mt19937 generator(rd());
        std::normal_distribution<float> normal(0.0f, 1.0f);

        path[0] = initial_price;

        for (int i = 1; i <= steps; i++) {
        float Z = normal(generator);

        path[i] = path[i - 1] * std::exp((drift - 0.5f * vol * vol) * dt +
                                        vol * std::sqrt(dt) * Z);
        }

        return path;
    }
};
