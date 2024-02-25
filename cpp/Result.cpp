#include "Result.h"

#include "SQLException.h"
#include <sql.h>
#include <sqlext.h>
#include <algorithm>
#include <utility>

Result::Result(Result&& x) : stmt(std::exchange(x.stmt, INVALID_STMT_HANDLE))
{
}

Result::~Result()
{
    close();
}

Result& Result::operator=(Result&& x)
{
    Result tmp = std::move(x);
    std::swap(*this, tmp);
    return *this;
}

const std::vector<std::string>& Result::getColumnNames() const
{
    return columnNames;
}

bool Result::next()
{
    return SQL_SUCCEEDED(SQLFetch(stmt));
}

std::shared_ptr<int64_t> Result::getInt(size_t column) const
{
    return get<int64_t>(column, SQL_C_SBIGINT);
}

std::shared_ptr<int64_t> Result::getInt(const std::string& column) const
{
    return get<int64_t>(column, SQL_C_SBIGINT);
}

std::shared_ptr<double> Result::getFloat(size_t column) const
{
    return get<double>(column, SQL_C_DOUBLE);
}

std::shared_ptr<double> Result::getFloat(const std::string& column) const
{
    return get<double>(column, SQL_C_DOUBLE);
}

std::shared_ptr<std::string> Result::getString(size_t column) const
{
    // get length first
    char c;
    SQLLEN len;
    SQLRETURN ret = SQLGetData(stmt, column + 1, SQL_C_CHAR, &c, 0, &len);
    if (!SQL_SUCCEEDED(ret))
    {
        throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
    }

    // check for NULL
    if (len == SQL_NULL_DATA)
    {
        return {};
    }

    // now get data
    std::shared_ptr<std::string> result = std::make_shared<std::string>(len + 1, '\0');
    ret = SQLGetData(stmt, column + 1, SQL_C_CHAR, result->data(), len + 1, &len);
    if (!SQL_SUCCEEDED(ret))
    {
        throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
    }
    return result;
}

std::shared_ptr<std::string> Result::getString(const std::string& column) const
{
    size_t index = getColumnByName(column);
    return getString(index);
}

void Result::close()
{
    if (stmt != INVALID_STMT_HANDLE)
    {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt); stmt = INVALID_STMT_HANDLE;
    }
}

Result::Result(SQLHSTMT stmt) : stmt(stmt)
{
    SQLSMALLINT columnCount = 0;
    SQLRETURN ret = SQLNumResultCols(stmt, &columnCount);
    if (!SQL_SUCCEEDED(ret))
    {
        throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
    }
    for (SQLSMALLINT i = 0; i < columnCount; ++i)
    {
        SQLCHAR name[256];
        SQLSMALLINT nameLen;
        SQLSMALLINT type;
        SQLULEN size;
        SQLSMALLINT decimalDigits;
        SQLSMALLINT nullable;
        ret = SQLDescribeColA(stmt, i + 1, name, sizeof(name), &nameLen, &type, &size, &decimalDigits, &nullable);
        if (!SQL_SUCCEEDED(ret))
        {
            throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
        }
        std::string columnName(name, name + nameLen);
        columnNames.push_back(std::move(columnName));
    }
}

size_t Result::getColumnByName(const std::string& column) const
{
    size_t index = std::find(columnNames.begin(), columnNames.end(), column) - columnNames.begin();
    if (index >= columnNames.size() || columnNames[index] != column)
    {
        throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
    }
    return index;
}

template <class T>
std::shared_ptr<T> Result::get(size_t column, SQLSMALLINT type) const
{
    // get data
    T result;
    SQLLEN len;
    SQLRETURN ret = SQLGetData(stmt, column + 1, type, &result, 0, &len);
    if (!SQL_SUCCEEDED(ret))
    {
        throw SQLException(__FILE__ + (": " + std::to_string(__LINE__)));
    }

    // check for NULL
    if (len == SQL_NULL_DATA)
    {
        return {};
    }

    return std::make_shared<T>(std::move_if_noexcept(result));
}

template <class T>
std::shared_ptr<T> Result::get(const std::string& column, SQLSMALLINT type) const
{
    size_t index = getColumnByName(column);
    return get<T>(index, type);
}

const SQLHSTMT Result::INVALID_STMT_HANDLE = SQLHSTMT(SQL_INVALID_HANDLE);
