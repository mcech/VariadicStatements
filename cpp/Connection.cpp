#include "Connection.h"

#include <utility>

Connection::Connection(const std::string& url, const std::string& user, const std::string& passwd)
{
    constexpr SQLCHAR UID[] = ";UID=";
    constexpr SQLCHAR PWD[] = ";PWD=";

    std::basic_string<SQLCHAR> url_user_passwd(url.begin(), url.end());
    url_user_passwd.append(UID);
    url_user_passwd.append(user.begin(), user.end());
    url_user_passwd.append(PWD);
    url_user_passwd.append(passwd.begin(), passwd.end());

    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    if (!SQL_SUCCEEDED(ret))
    {
        throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
    }

    ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, SQLPOINTER(SQL_OV_ODBC3), 0);
    if (!SQL_SUCCEEDED(ret))
    {
        throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
    }

    ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    if (!SQL_SUCCEEDED(ret))
    {
        throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
    }

    ret = SQLDriverConnectA(
            dbc,
            NULL,
            url_user_passwd.data(),
            url_user_passwd.length(),
            NULL,
            0,
            NULL,
            SQL_DRIVER_COMPLETE);
    if (!SQL_SUCCEEDED(ret))
    {
        throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
    }
}

Connection::Connection(Connection&& x) : env(std::exchange(x.env, INVALID_ENV_HANDLE)), dbc(std::exchange(x.dbc, INVALID_DBC_HANDLE))
{
}

Connection::~Connection()
{
    close();
}

Connection& Connection::operator=(Connection&& x)
{
    Connection tmp = std::move(x);
    std::swap(*this, tmp);
    return *this;
}

void Connection::begin()
{
    setAutoCommit(false);
}

void Connection::commit()
{
    SQLEndTran(SQL_HANDLE_DBC, dbc, SQL_COMMIT);
    setAutoCommit(true);
}

void Connection::rollback()
{
    SQLEndTran(SQL_HANDLE_DBC, dbc, SQL_ROLLBACK);
    setAutoCommit(true);
}

void Connection::close()
{
    if (dbc != INVALID_DBC_HANDLE)
    {
        SQLDisconnect(dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc); dbc = INVALID_DBC_HANDLE;
        SQLFreeHandle(SQL_HANDLE_ENV, env); env = INVALID_ENV_HANDLE;
    }
}

void Connection::setAutoCommit(bool val)
{
    SQLRETURN ret = SQLSetConnectAttrA(dbc, SQL_ATTR_AUTOCOMMIT, SQLPOINTER(val), 0);
    if (!SQL_SUCCEEDED(ret))
    {
        throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
    }
}

inline void Connection::bind(SQLHSTMT stmt, size_t i)
{
}

const SQLHENV Connection::INVALID_ENV_HANDLE = SQLHENV(SQL_INVALID_HANDLE);
const SQLHDBC Connection::INVALID_DBC_HANDLE = SQLHDBC(SQL_INVALID_HANDLE);
