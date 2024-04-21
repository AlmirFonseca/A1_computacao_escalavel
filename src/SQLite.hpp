#ifndef SQLITE_HPP
#define SQLITE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <sqlite3.h>

#include "DataFrame.hpp"


/**
 * SQLite class for handling SQLite operations.
 */
class SQLite {
private:

    sqlite3* db_; ///< SQLite database connection
    std::string dbFilename_; ///< Filename of the SQLite database

public:
    
    /**
     * Constructor.
     * @param dbFilename The filename of the SQLite database.
     */
    SQLite(const std::string& dbFilename) : dbFilename_(dbFilename) {}
    
    /**
     * Destructor.
     */
    ~SQLite() {
        if (db_) {
            sqlite3_close(db_);
        }
    }
    
    /**
     * @brief Creates tables for the 4 entities in mock: product, purchase_order, stock and user
     */
    void createTables() {
        // Open SQLite database
        int rc = sqlite3_open(dbFilename_.c_str(), &db_);
        if (rc != SQLITE_OK) {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db_) << std::endl;
            sqlite3_close(db_);
            return;
        }
        
        // Create tables product if they don't exist
        createTable("product", "id TEXT, name TEXT, image TEXT, description TEXT, price REAL");
        createTable("purchase_order", "user_id TEXT, product_id TEXT, quantity INTEGER, creation_date TEXT, payment_date TEXT, delivery_date TEXT");
        createTable("stock", "id_product TEXT, quantity INTEGER");
        createTable("user", "id TEXT, name TEXT, email TEXT, address TEXT, registration_date TEXT, birth_date TEXT");
        
        // Close SQLite database
        sqlite3_close(db_);
    }
    
    /**
     * @brief Inserts a user instance into the user table.
     * 
     * @param df The DataFrame containing the user data.
     */
    void insertUser(DataFrame& df) {
        // Iterate over each row in the DataFrame
        for (int rowIndex = 0; rowIndex < df.getRowCount(); rowIndex++) {
            // Build SQL statement
            std::string sql = "INSERT INTO user VALUES ('";
            for (int columnIndex = 0; columnIndex < df.getColumnCount(); columnIndex++) {
                sql += df.getValueAt(rowIndex, columnIndex);
                if (j < df.getColumnCount() - 1) {
                    sql += "', '";
                }
            }
            sql += "');";

            // Execute SQL statement
            executeSQL(sql);
        }

        // Close SQLite database
        sqlite3_close(db_);
    }

    /**
     * @brief Inserts a product instance into the product table.
     * 
     * @param df The DataFrame containing the product data.
     */
    void insertProduct(DataFrame& df) {
        // Iterate over each row in the DataFrame
        for (int rowIndex = 0; rowIndex < df.getRowCount(); rowIndex++) {
            // Build SQL statement
            std::string sql = "INSERT INTO product VALUES ('";
            for (int columnIndex = 0; columnIndex < df.getColumnCount(); columnIndex++) {
                sql += df.getValueAt(rowIndex, columnIndex);
                if (j < df.getColumnCount() - 1) {
                    sql += "', '";
                }
            }
            sql += "');";

            // Execute SQL statement
            executeSQL(sql);
        }

        // Close SQLite database
        sqlite3_close(db_);
    }

    /**
     * @brief Inserts a purchase order instance into the purchase_order table.
     * 
     * @param df The DataFrame containing the purchase order data.
     */
    void insertPurchaseOrder(DataFrame& df) {
        // Iterate over each row in the DataFrame
        for (int rowIndex = 0; rowIndex < df.getRowCount(); rowIndex++) {
            // Build SQL statement
            std::string sql = "INSERT INTO purchase_order VALUES ('";
            for (int columnIndex = 0; columnIndex < df.getColumnCount(); columnIndex++) {
                sql += df.getValueAt(rowIndex, columnIndex);
                if (j < df.getColumnCount() - 1) {
                    sql += "', '";
                }
            }
            sql += "');";

            // Execute SQL statement
            executeSQL(sql);
        }

        // Close SQLite database
        sqlite3_close(db_);
    }

    /**
     * @brief Inserts a stock instance into the stock table.
     * 
     * @param df The DataFrame containing the stock data.
     */
    void insertStock(DataFrame& df) {
        // Iterate over each row in the DataFrame
        for (int rowIndex = 0; rowIndex < df.getRowCount(); rowIndex++) {
            // Build SQL statement
            std::string sql = "INSERT INTO stock VALUES ('";
            for (int columnIndex = 0; columnIndex < df.getColumnCount(); columnIndex++) {
                sql += df.getValueAt(rowIndex, columnIndex);
                if (j < df.getColumnCount() - 1) {
                    sql += "', '";
                }
            }
            sql += "');";

            // Execute SQL statement
            executeSQL(sql);
        }

        // Close SQLite database
        sqlite3_close(db_);
    }

    /**
     * @brief Updates a stock in the stock table.
     * 
     * @param df The DataFrame containing the stock data.
     */
    void updateStock(DataFrame& df) {
        // Iterate over each row in the DataFrame
        for (int rowIndex = 0; rowIndex < df.getRowCount(); rowIndex++) {
            // Build SQL statement
            std::string sql = "UPDATE stock SET quantity = " + df.getValueAt(rowIndex, 1) + " WHERE id_product = '" + df.getValueAt(rowIndex, 0) + "';";

            // Execute SQL statement
            executeSQL(sql);
        }

        // Close SQLite database
        sqlite3_close(db_);
    }


private:
    
    sqlite3* db_; ///< SQLite database connection
    std::string dbFilename_; ///< Filename of the SQLite database
    
    /**
     * @brief Executes an SQL statement.
     * 
     * @param sql The SQL statement to execute.
     * @return True if execution is successful, false otherwise.
     */
    bool executeSQL(const std::string& sql) {
        int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(db_) << std::endl;
            return false;
        }
        return true;
    }
    
    
    /**
     * @brief Creates a table with the given name and columns.
     * 
     * @param tableName The name of the table to create.
     * @param columns The columns of the table in the format "column1Type column1Name, column2Type column2Name, ...".
     * @return True if table creation is successful, false otherwise.
     */
    bool createTable(const std::string& tableName, const std::string& columns) {
        std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (" + columns + ");";
        return executeSQL(sql);
    }


};

#endif // SQLITE_HPP
