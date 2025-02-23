#ifndef MYDATASTORE_H
#define MYDATASTORE_H
#include "datastore.h"
#include "product.h"
#include "user.h"
#include <map>
#include <vector>
#include <set>
#include <string>

class MyDataStore : public DataStore {
  public:
    MyDataStore();
    ~MyDataStore();

    void addProduct(Product* p) override;
    void addUser(User* u) override;
    std::vector<Product*> search(std::vector<std::string>& terms, int type) override;
    void dump(std::ostream& ofile) override;

    void addToCart(const std::string& username, Product* p);
    void viewCart(const std::string& username);
    void checkout(const std::string& username);

    bool userExists(const std::string& username) const;

  private:

    void indexProductKeywords(Product* p);

    std::map<std::string, User*> users_;
    std::vector<Product*> products_;
    std::map<std::string, std::set<Product*>> keywordIndex_;
    std::map<std::string, std::vector<Product*>> userCarts_;
};

#endif