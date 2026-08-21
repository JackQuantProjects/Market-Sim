#pragma once

#include <cstddef>
#include <iostream>

#include "Parameters.cpp"
#include "Inventory.cpp"
#include "Stoikov.cpp"
#include "Draw.cpp"
#include "Poisson.cpp"

class Sim {
private:
    Parameters parameters;
    Inventory inventory;
    Stoikov stoikov;
    Draw draw;
    Poisson poisson;


public:
    Sim()
        : parameters(),
          inventory(0),
          stoikov(
              parameters.getVol(),
              parameters.getRiskAversion(),
              parameters.getLiquidity(),
              parameters.getDuration(),
              parameters.getSteps(),
              inventory
          ),
          draw(parameters)
    {
    }

    Parameters& getParameters()
    {
        return parameters;
    }

    Inventory& getInventory()
    {
        return inventory;
    }

    Stoikov& getStoikov()
    {
        return stoikov;
    }

    void startSim() {
        std::cout << "Starting simulation\n";

        stoikov.reset();
        parameters.reset();
        inventory.reset();

        parameters.createMarketPath();

        stoikov.setParams(
            parameters.getVol(),
            parameters.getRiskAversion(),
            parameters.getLiquidity(),
            parameters.getDuration(),
            parameters.getSteps()
        );

        poisson.setParams(
            parameters.getDuration(),
            parameters.getIntensity(),
            parameters.getSteps()
        );

        const std::vector<float>& path = parameters.getPath();

        for (size_t i = 0; i < path.size(); i++)
        {
            step(i);
        }

        std::cout << "Simulation complete\n";
    }

    void step(size_t i)
    {
        float mid = parameters.getPath()[i];

        stoikov.quotes(i, mid);

        float bidDist = stoikov.getBidDist();
        float askDist = stoikov.getAskDist();

        int buyFills =
            poisson.sample(
                bidDist,
                parameters.getLiquidity()
            );

        int sellFills =
            poisson.sample(
                askDist,
                parameters.getLiquidity()
            );

        inventory.buy(
            buyFills,
            stoikov.currentBid()
        );

        inventory.sell(
            sellFills,
            stoikov.currentAsk()
        );

        std::cout
            << "Mid: " << mid
            << " | Bid: " << stoikov.currentBid()
            << " | Ask: " << stoikov.currentAsk()
            << " | BidDist: " << bidDist
            << " | AskDist: " << askDist
            << " | Buy: " << buyFills
            << " | Sell: " << sellFills
            << " | Inventory: " << inventory.getQuantity()
            << "\n";
    }

    void run()
{
    if (!draw.Initialise())
        return;

    while (draw.IsOpen())
    {
        draw.BeginFrame();

        // Draw all UI
        draw.init();

        // Check whether user clicked Start
        if (draw.StartPressed())
        {
            startSim();
        }

        // Check Reset
        if (draw.ResetPressed())
        {
            std::cout << "Resetting\n";
            stoikov.reset();
            parameters.reset();
            inventory.reset();
        }

        // Draw simulation results
        draw.BeginMarketWindow();

        draw.DrawMarket(
            parameters.getPath(),
            stoikov.getBid(),
            stoikov.getAsk()
        );

        draw.EndMarketWindow();

        draw.BeginInventoryWindow();

        draw.DrawInventory(
            inventory.getQuantity()
        );

        draw.EndInventoryWindow();

        draw.BeginPNLWindow();

        // Replace with draw.DrawPNL(inventory.getPNL()) once
        // fills carry their price through to a cash balance.
        draw.DrawPNLUnavailable();

        draw.EndPNLWindow();

        draw.EndFrame();
    }

    draw.Shutdown();
}
};
