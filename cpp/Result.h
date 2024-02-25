#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif
#include <sql.h>
#include <sqltypes.h>
#include <memory>
#include <string>
#include <vector>
#include <cstddef>

/**
 * A table of data  representing a database result,  which is  usually generated
 * by executing a statement that queries the database.
 *
 * A Result  object  maintains  a cursor  pointing to  its current  row of data.
 * Initially  the cursor is  positioned before the first row.  The next() method
 * moves the cursor to the next row, and because it returns false when there are
 * no more rows in the Result object,  it can be used in a while loop to iterate
 * through the result set.
 */
class Result
{
public:
    /**
     * Creates a new Result object which actually does not represent a result.
     */
    Result() = default;

    Result(const Result&) = delete;

    /**
     * Move constructor
     */
    Result(Result&& x);

    /**
     * Destructor
     */
    ~Result();

    Result& operator=(const Result&) = delete;

    /**
     * Move assignment
     */
    Result& operator=(Result&& x);

    /**
     * Get the column names.
     *
     * @return An array containing the suggested column names
     *
     * @throws SQLException  If a database access error occurs
     */
    const std::vector<std::string>& getColumnNames() const;

    /**
     * Moves the  cursor forward  one row from  its current  position.  A Result
     * cursor is initially  positioned before  the first row;  the first call to
     * the method  next makes the  first row the  current row;  the second  call
     * makes the second row the current row, and so on.
     *
     * @return true          If the new current row is valid
     *         false         If there are no more rows
     *
     * @throws SQLException  If a database access error occurs
     */
    bool next();

    /**
     * Gets the value of the designated column in the current row of this Result
     * object.
     *
     * @param  column        The first column is 0, the second is 1, ...
     *
     * @return An Object holding the column value
     *
     * @throws SQLException  If a database access error occurs
     */
    std::shared_ptr<int64_t> getInt(size_t column) const;

    /**
     * Gets the value of the designated column in the current row of this Result
     * object.
     *
     * @param  column        The label for the column specified  with the SQL AS
     *                       clause.  If  the SQL AS clause  was  not specified,
     *                       then the label is the name of the column.
     *
     * @return An Object holding the column value
     *
     * @throws SQLException  If a database access error occurs
     */
    std::shared_ptr<int64_t> getInt(const std::string& column) const;

    /**
     * Gets the value of the designated column in the current row of this Result
     * object.
     *
     * @param  column        The first column is 0, the second is 1, ...
     *
     * @return An Object holding the column value
     *
     * @throws SQLException  If a database access error occurs
     */
    std::shared_ptr<double> getFloat(size_t column) const;

    /**
     * Gets the value of the designated column in the current row of this Result
     * object.
     *
     * @param  column        The label for the column specified  with the SQL AS
     *                       clause.  If  the SQL AS clause  was  not specified,
     *                       then the label is the name of the column.
     *
     * @return An Object holding the column value
     *
     * @throws SQLException  If a database access error occurs
     */
    std::shared_ptr<double> getFloat(const std::string& column) const;

    /**
     * Gets the value of the designated column in the current row of this Result
     * object.
     *
     * @param  column        The first column is 0, the second is 1, ...
     *
     * @return An Object holding the column value
     *
     * @throws SQLException  If a database access error occurs
     */
    std::shared_ptr<std::string> getString(size_t column) const;

    /**
     * Gets the value of the designated column in the current row of this Result
     * object.
     *
     * @param  column        The label for the column specified  with the SQL AS
     *                       clause.  If  the SQL AS clause  was  not specified,
     *                       then the label is the name of the column.
     *
     * @return An Object holding the column value
     *
     * @throws SQLException  If a database access error occurs
     */
    std::shared_ptr<std::string> getString(const std::string& column) const;

    /**
     * Closes the Result and releases its resources.
     */
    void close();

private:
    friend class Connection;

    Result(SQLHSTMT stmt);
    size_t getColumnByName(const std::string& column) const;
    template <class T> std::shared_ptr<T> get(size_t column, SQLSMALLINT type) const;
    template <class T> std::shared_ptr<T> get(const std::string& column, SQLSMALLINT type) const;

    static const SQLHSTMT INVALID_STMT_HANDLE;

    SQLHSTMT stmt = INVALID_STMT_HANDLE;
    std::vector<std::string> columnNames;
};
