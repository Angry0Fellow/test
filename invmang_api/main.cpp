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

  // Route to add an object
  CROW_ROUTE(app, "/api/add_object")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto params = crow::json::load(req.body);
        if (!params)
          return crow::response(400, "Invalid JSON");

        std::string serial = params["serial"].s();
        std::string name = params["name"].s();

        try {
          sql::mysql::MySQL_Driver *driver;
          sql::Connection *con;

          driver = sql::mysql::get_mysql_driver_instance();
          con = driver->connect("tcp://" + DB_HOST + ":3306", DB_USER, DB_PASS);
          con->setSchema(DB_NAME);

          std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
              "INSERT INTO objects (serial, name) VALUES (?, ?)"));
          pstmt->setString(1, serial);
          pstmt->setString(2, name);
          pstmt->execute();

          delete con;

          crow::json::wvalue result;
          result["message"] = "Object added successfully.";
          return crow::response(200, result);
        } catch (sql::SQLException &e) {
          return crow::response(500, std::string("Error adding object: ") +
                                         e.what());
        }
      });

  // Route to move an object (simplified example)
  CROW_ROUTE(app, "/api/move_object")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto params = crow::json::load(req.body);
        if (!params)
          return crow::response(400, "Invalid JSON");

        int parent_id = params["parent_id"].i();
        int child_id = params["child_id"].i();

        try {
          sql::mysql::MySQL_Driver *driver;
          sql::Connection *con;

          driver = sql::mysql::get_mysql_driver_instance();
          con = driver->connect("tcp://" + DB_HOST + ":3306", DB_USER, DB_PASS);
          con->setSchema(DB_NAME);

          // Delete existing relationship
          std::unique_ptr<sql::PreparedStatement> pstmt_delete(
              con->prepareStatement(
                  "DELETE FROM relationships WHERE child_id = ?"));
          pstmt_delete->setInt(1, child_id);
          pstmt_delete->execute();

          // Insert new relationship
          std::unique_ptr<sql::PreparedStatement> pstmt_insert(
              con->prepareStatement("INSERT INTO relationships (parent_id, "
                                    "child_id) VALUES (?, ?)"));
          pstmt_insert->setInt(1, parent_id);
          pstmt_insert->setInt(2, child_id);
          pstmt_insert->execute();

          delete con;

          crow::json::wvalue result;
          result["message"] = "Object moved successfully.";
          return crow::response(200, result);
        } catch (sql::SQLException &e) {
          return crow::response(500, std::string("Error moving object: ") +
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
      result["objects"] = crow::json::wvalue::list();

      while (res->next()) {
        crow::json::wvalue obj;
        obj["id"] = res->getInt("id");
        obj["serial"] = res->getString("serial");
        obj["name"] = res->getString("name");
        result["objects"].push_back(obj);
      }

      delete con;

      return crow::response(200, result);
    } catch (sql::SQLException &e) {
      return crow::response(500,
                            std::string("Error fetching objects: ") + e.what());
    }
  });

  // Start the server
  app.port(18080).multithreaded().run();

  return 0;
}
