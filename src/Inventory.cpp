class Inventory {
private:
  int q; // quantity
public:
  Inventory(int initq) { q = initq; }
  void buy(int amount, float price) { q += amount; }
  void sell(int amount, float price) { q -= amount; }
  int getQuantity() const { return q; }
};
