class Inventory {
private:
  int q; // quantity
public:
  Inventory(int initq) { q = initq; }
  void buy(int amount) { q += amount; }
  void sell(int amount) { q -= amount; }
  int getQuantity() const { return q; }
};
