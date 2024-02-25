#include "Connection.h"
#include <iostream>

int main()
{
    Connection con("DRIVER=MariaDB Unicode;SERVER=localhost;DATABASE=********", "****", "****");
    Result rs = con.executeQuery("SELECT `balance` FROM `accounts`");
    while (rs.next())
    {
        std::cout << *rs.getFloat(0) << std::endl;
    }

    return 0;
}
