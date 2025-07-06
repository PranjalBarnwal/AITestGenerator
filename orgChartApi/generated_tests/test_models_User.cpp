#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/orm/DbClient.h>
#include "../models/User.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;

class UserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(UserTest, DefaultConstructor) {
    User user;
    EXPECT_FALSE(user.getId());
    EXPECT_FALSE(user.getUsername());
    EXPECT_FALSE(user.getPassword());
}

TEST_F(UserTest, SetAndGetId) {
    User user;
    user.setId(1);
    ASSERT_TRUE(user.getId());
    EXPECT_EQ(user.getValueOfId(), 1);
}

TEST_F(UserTest, SetAndGetUsername) {
    User user;
    user.setUsername("testuser");
    ASSERT_TRUE(user.getUsername());
    EXPECT_EQ(user.getValueOfUsername(), "testuser");
}

TEST_F(UserTest, SetAndGetPassword) {
    User user;
    user.setPassword("password123");
    ASSERT_TRUE(user.getPassword());
    EXPECT_EQ(user.getValueOfPassword(), "password123");
}

TEST_F(UserTest, JsonConstruction) {
    Json::Value json;
    json["id"] = 1;
    json["username"] = "testuser";
    json["password"] = "password123";
    
    User user(json);
    EXPECT_EQ(user.getValueOfId(), 1);
    EXPECT_EQ(user.getValueOfUsername(), "testuser");
    EXPECT_EQ(user.getValueOfPassword(), "password123");
}

TEST_F(UserTest, ToJson) {
    User user;
    user.setId(1);
    user.setUsername("testuser");
    user.setPassword("password123");
    
    Json::Value json = user.toJson();
    EXPECT_EQ(json["id"].asInt(), 1);
    EXPECT_EQ(json["username"].asString(), "testuser");
    EXPECT_EQ(json["password"].asString(), "password123");
}

TEST_F(UserTest, ValidateJsonForCreation_Valid) {
    Json::Value json;
    json["username"] = "testuser";
    json["password"] = "password123";
    
    std::string err;
    EXPECT_TRUE(User::validateJsonForCreation(json, err));
    EXPECT_TRUE(err.empty());
}

TEST_F(UserTest, ValidateJsonForCreation_Invalid) {
    Json::Value json;
    // Missing required fields
    
    std::string err;
    EXPECT_FALSE(User::validateJsonForCreation(json, err));
    EXPECT_FALSE(err.empty());
}

TEST_F(UserTest, ValidateJsonForUpdate_Valid) {
    Json::Value json;
    json["username"] = "newtestuser";
    
    std::string err;
    EXPECT_TRUE(User::validateJsonForUpdate(json, err));
    EXPECT_TRUE(err.empty());
}

TEST_F(UserTest, MasqueradedJson) {
    User user;
    user.setId(1);
    user.setUsername("testuser");
    user.setPassword("password123");
    
    std::vector<std::string> masquerading = {"user_id", "user_name", "user_password"};
    Json::Value json = user.toMasqueradedJson(masquerading);
    
    EXPECT_EQ(json["user_id"].asInt(), 1);
    EXPECT_EQ(json["user_name"].asString(), "testuser");
    EXPECT_EQ(json["user_password"].asString(), "password123");
}

TEST_F(UserTest, TableMetadata) {
    EXPECT_EQ(User::tableName, "users");
    EXPECT_TRUE(User::hasPrimaryKey);
    EXPECT_EQ(User::primaryKeyName, "id");
    EXPECT_EQ(User::getColumnNumber(), 3);
}

TEST_F(UserTest, ColumnNames) {
    EXPECT_EQ(User::Cols::_id, "id");
    EXPECT_EQ(User::Cols::_username, "username");
    EXPECT_EQ(User::Cols::_password, "password");
    
    EXPECT_EQ(User::getColumnName(0), "id");
    EXPECT_EQ(User::getColumnName(1), "username");
    EXPECT_EQ(User::getColumnName(2), "password");
}

TEST_F(UserTest, SqlQueries) {
    EXPECT_FALSE(User::sqlForFindingByPrimaryKey().empty());
    EXPECT_FALSE(User::sqlForDeletingByPrimaryKey().empty());
    
    User user;
    bool needSelection;
    EXPECT_FALSE(user.sqlForInserting(needSelection).empty());
}

TEST_F(UserTest, MoveStrings) {
    User user;
    std::string username = "testuser";
    std::string password = "password123";
    user.setUsername(std::move(username));
    user.setPassword(std::move(password));
    
    EXPECT_EQ(user.getValueOfUsername(), "testuser");
    EXPECT_EQ(user.getValueOfPassword(), "password123");
    EXPECT_TRUE(username.empty()); // Original strings should be moved from
    EXPECT_TRUE(password.empty());
}

TEST_F(UserTest, UpdateByJson) {
    User user;
    user.setUsername("olduser");
    user.setPassword("oldpass");
    
    Json::Value updateJson;
    updateJson["username"] = "newuser";
    updateJson["password"] = "newpass";
    
    user.updateByJson(updateJson);
    EXPECT_EQ(user.getValueOfUsername(), "newuser");
    EXPECT_EQ(user.getValueOfPassword(), "newpass");
}

TEST_F(UserTest, UpdateByMasqueradedJson) {
    User user;
    user.setUsername("olduser");
    user.setPassword("oldpass");
    
    Json::Value updateJson;
    updateJson["user_name"] = "newuser";
    updateJson["user_password"] = "newpass";
    
    std::vector<std::string> masquerading = {"user_name", "user_password"};
    user.updateByMasqueradedJson(updateJson, masquerading);
    
    EXPECT_EQ(user.getValueOfUsername(), "newuser");
    EXPECT_EQ(user.getValueOfPassword(), "newpass");
} 