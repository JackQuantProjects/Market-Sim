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
              parameters.getIntensity(),
              parameters.getDuration(),
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

        std::cout << "About to create path\n";

        parameters.createMarketPath();

        std::cout << "Path created\n";

        std::cout << "About to set Stoikov params\n";

        stoikov.setParams(
            parameters.getVol(),
            parameters.getRiskAversion(),
            parameters.getLiquidity(),
            parameters.getIntensity(),
            parameters.getDuration()
        );

        poisson.setParams(parameters.getDuration(), parameters.getIntensity());

        std::cout << "Stoikov params set\n";

        for (int i = 0; i < parameters.getPath().size(); i++)
        {
            step(i);
        }

        std::cout << "Simulation complete\n";
    }

    void step(int i)
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
        }

        // Draw simulation results
        if (!parameters.getPath().empty())
        {
            draw.BeginMarketWindow();

            draw.DrawMarket(
                parameters.getPath(),
                stoikov.getBid(),
                stoikov.getAsk()
            );

            draw.EndMarketWindow();
        }

        draw.BeginInventoryWindow();

        draw.DrawInventory(
            inventory.getQuantity()
        );

        draw.EndInventoryWindow();

        draw.BeginPNLWindow();

        // whatever your PNL getter is
        // draw.DrawPNL(inventory.getPNL());

        draw.EndPNLWindow();

        draw.EndFrame();
    }

    draw.Shutdown();
}
};