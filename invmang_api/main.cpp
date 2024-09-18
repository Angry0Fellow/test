#include "crow_all.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <mysql_connection.h>
#include <mysql_driver.h>

// Database credentials
const std::string DB_HOST = "db";
const std::string DB_USER = "invuser";
const std::string DB_PASS = "invpassword";
const std::string DB_NAME = "invmang";

int main() {
  crow::SimpleApp app;

  // Route to add an object with optional parent_id
  CROW_ROUTE(app, "/api/add_object")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto params = crow::json::load(req.body);
        if (!params)
          return crow::response(400, "Invalid JSON");

        std::string serial = params["serial"].s();
        std::string name = params["name"].s();

        sql::Connection *con = nullptr; // Initialize con to nullptr

        try {
          sql::mysql::MySQL_Driver *driver;

          driver = sql::mysql::get_mysql_driver_instance();
          con = driver->connect("tcp://" + DB_HOST + ":3306", DB_USER, DB_PASS);
          con->setSchema(DB_NAME);

          con->setAutoCommit(false); // Start transaction

          // Insert into objects table
          std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
              "INSERT INTO objects (serial, name) VALUES (?, ?)"));
          pstmt->setString(1, serial);
          pstmt->setString(2, name);
          pstmt->execute();

          // Get the ID of the newly inserted object
          std::unique_ptr<sql::ResultSet> res(pstmt->getGeneratedKeys());
          int new_object_id = 0;
          if (res->next()) {
            new_object_id = res->getInt(1);
          }

          // Insert into relationships table, if parent_id is provided
          if (params.has("parent_id")) {
            int parent_id = 0;
            auto parent_value = params["parent_id"];

            // Check the type of parent_id
            if (parent_value.t() == crow::json::type::Integer) {
              parent_id = parent_value.i();
            } else if (parent_value.t() == crow::json::type::Double) {
              parent_id = static_cast<int>(parent_value.d());
            } else {
              con->rollback();
              delete con;
              return crow::response(400, "Invalid parent_id type");
            }

            // Optionally validate parent_id exists
            std::unique_ptr<sql::PreparedStatement> pstmt_check(
                con->prepareStatement(
                    "SELECT COUNT(*) FROM objects WHERE id = ?"));
            pstmt_check->setInt(1, parent_id);
            std::unique_ptr<sql::ResultSet> res_check(
                pstmt_check->executeQuery());
            res_check->next();
            if (res_check->getInt(1) == 0) {
              con->rollback();
              delete con;
              return crow::response(400, "Invalid parent_id");
            }

            std::unique_ptr<sql::PreparedStatement> pstmt_rel(
                con->prepareStatement("INSERT INTO relationships (parent_id, "
                                      "child_id) VALUES (?, ?)"));
            pstmt_rel->setInt(1, parent_id);
            pstmt_rel->setInt(2, new_object_id);
            pstmt_rel->execute();
          }

          con->commit(); // Commit transaction
          delete con;

          crow::json::wvalue result;
          result["message"] = "Object added successfully.";
          result["id"] = new_object_id;
          return crow::response(200, result);
        } catch (sql::SQLException &e) {
          if (con) {
            con->rollback(); // Rollback transaction on error
            delete con;
          }
          return crow::response(500, std::string("Error adding object: ") +
                                         e.what());
        }
      });

  // Route to get all objects
  CROW_ROUTE(app, "/api/objects").methods(crow::HTTPMethod::GET)([]() {
    try {
      sql::mysql::MySQL_Driver *driver;
      sql::Connection *con;

      driver = sql::mysql::get_mysql_driver_instance();
      con = driver->connect("tcp://" + DB_HOST + ":3306", DB_USER, DB_PASS);
      con->setSchema(DB_NAME);

      std::unique_ptr<sql::Statement> stmt(con->createStatement());
      std::unique_ptr<sql::ResultSet> res(
          stmt->executeQuery("SELECT * FROM objects"));

      crow::json::wvalue result;
      crow::json::wvalue::list objects_list;

      // Iterate through the database results and construct the JSON list
      while (res->next()) {
        crow::json::wvalue obj;
        obj["id"] = res->getInt("id");
        obj["serial"] = res->getString("serial");
        obj["name"] = res->getString("name");
        objects_list.emplace_back(obj); // Add each object to the list
      }

      result["objects"] =
          std::move(objects_list); // Move the list into the result

      delete con;

      return crow::response(200, result);
    } catch (sql::SQLException &e) {
      return crow::response(500,
                            std::string("Error fetching objects: ") + e.what());
    }
  });

  // Route to get all relationships
  CROW_ROUTE(app, "/api/relationships").methods(crow::HTTPMethod::GET)([]() {
    try {
      sql::mysql::MySQL_Driver *driver;
      sql::Connection *con;

      driver = sql::mysql::get_mysql_driver_instance();
      con = driver->connect("tcp://" + DB_HOST + ":3306", DB_USER, DB_PASS);
      con->setSchema(DB_NAME);

      std::unique_ptr<sql::Statement> stmt(con->createStatement());
      std::unique_ptr<sql::ResultSet> res(
          stmt->executeQuery("SELECT * FROM relationships"));

      crow::json::wvalue result;
      crow::json::wvalue::list relationships_list;

      // Iterate through the database results and construct the JSON list
      while (res->next()) {
        crow::json::wvalue obj;
        obj["parent_id"] = res->getInt("parent_id");
        obj["child_id"] = res->getInt("child_id");
        relationships_list.emplace_back(obj); // Add each object to the list
      }

      result["relationships"] =
          std::move(relationships_list); // Move the list into the result

      delete con;

      return crow::response(200, result);
    } catch (sql::SQLException &e) {
      return crow::response(500, std::string("Error fetching relationships: ") +
                                     e.what());
    }
  });

  // Start the server
  app.port(18080).multithreaded().run();

  return 0;
}
