#include <iostream>

#include "Parameters.cpp"
#include "Inventory.cpp"
#include "Stoikov.cpp"
#include "Draw.cpp"

class Sim {
private:
    Parameters parameters;
    Inventory inventory;
    Stoikov stoikov;
    Draw draw;

public:
    Sim()
        : parameters(),
          inventory(-3000),
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

    void startSim()
    {
        std::cout << "Starting simulation\n";

        parameters.createMarketPath();
        std::cout << parameters.getVol();

        stoikov.setParams(
            parameters.getVol(),
            parameters.getRiskAversion(),
            parameters.getLiquidity(),
            parameters.getIntensity(),
            parameters.getDuration()
        );

        for (int i = 0; i < parameters.getPath().size(); i++)
        {
            step(i);
        }

        std::cout << "Simulation complete\n";
        for (auto item : stoikov.getBid()) {
            std::cout << item;
        }
        draw.DrawMarket(parameters.getPath(), stoikov.getBid(), stoikov.getAsk());
    }

    void step(int i)
    {
        stoikov.quotes(
            i,
            parameters.getPath()[i]
        );
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
            // reset simulation here
            std::cout << "Reset pressed\n";
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