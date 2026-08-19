class Sim {
private:
    Parameters parameters;
    Inventory inventory;

public:
    Sim()
        : parameters(),
          inventory(0)
    {
    }

    Parameters& getParameters() {
        return parameters;
    }

    Inventory& getInventory() {
        return inventory;
    }

    void startSim() {
        parameters.createMarketPath();

        // Trading simulation will go here
    }
};