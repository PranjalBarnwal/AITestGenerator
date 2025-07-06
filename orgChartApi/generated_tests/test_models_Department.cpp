#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/orm/DbClient.h>
#include "../models/Department.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;

class DepartmentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(DepartmentTest, DefaultConstructor) {
    Department dept;
    EXPECT_FALSE(dept.getId());  // Should be null/empty
    EXPECT_FALSE(dept.getName()); // Should be null/empty
}

TEST_F(DepartmentTest, SetAndGetId) {
    Department dept;
    dept.setId(1);
    ASSERT_TRUE(dept.getId());
    EXPECT_EQ(dept.getValueOfId(), 1);
}

TEST_F(DepartmentTest, SetAndGetName) {
    Department dept;
    dept.setName("Engineering");
    ASSERT_TRUE(dept.getName());
    EXPECT_EQ(dept.getValueOfName(), "Engineering");
}

TEST_F(DepartmentTest, JsonConstruction) {
    Json::Value json;
    json["id"] = 1;
    json["name"] = "Engineering";
    
    Department dept(json);
    EXPECT_EQ(dept.getValueOfId(), 1);
    EXPECT_EQ(dept.getValueOfName(), "Engineering");
}

TEST_F(DepartmentTest, ToJson) {
    Department dept;
    dept.setId(1);
    dept.setName("Engineering");
    
    Json::Value json = dept.toJson();
    EXPECT_EQ(json["id"].asInt(), 1);
    EXPECT_EQ(json["name"].asString(), "Engineering");
}

TEST_F(DepartmentTest, ValidateJsonForCreation_Valid) {
    Json::Value json;
    json["name"] = "Engineering";
    
    std::string err;
    EXPECT_TRUE(Department::validateJsonForCreation(json, err));
    EXPECT_TRUE(err.empty());
}

TEST_F(DepartmentTest, ValidateJsonForCreation_Invalid) {
    Json::Value json;
    // Missing required name field
    
    std::string err;
    EXPECT_FALSE(Department::validateJsonForCreation(json, err));
    EXPECT_FALSE(err.empty());
}

TEST_F(DepartmentTest, ValidateJsonForUpdate_Valid) {
    Json::Value json;
    json["name"] = "Updated Engineering";
    
    std::string err;
    EXPECT_TRUE(Department::validateJsonForUpdate(json, err));
    EXPECT_TRUE(err.empty());
}

TEST_F(DepartmentTest, MasqueradedJson) {
    Department dept;
    dept.setId(1);
    dept.setName("Engineering");
    
    std::vector<std::string> masquerading = {"department_id", "department_name"};
    Json::Value json = dept.toMasqueradedJson(masquerading);
    
    EXPECT_EQ(json["department_id"].asInt(), 1);
    EXPECT_EQ(json["department_name"].asString(), "Engineering");
}

TEST_F(DepartmentTest, TableMetadata) {
    EXPECT_EQ(Department::tableName, "departments");
    EXPECT_TRUE(Department::hasPrimaryKey);
    EXPECT_EQ(Department::primaryKeyName, "id");
    EXPECT_EQ(Department::getColumnNumber(), 2);
}

TEST_F(DepartmentTest, ColumnNames) {
    EXPECT_EQ(Department::Cols::_id, "id");
    EXPECT_EQ(Department::Cols::_name, "name");
    
    EXPECT_EQ(Department::getColumnName(0), "id");
    EXPECT_EQ(Department::getColumnName(1), "name");
}

TEST_F(DepartmentTest, SqlQueries) {
    EXPECT_FALSE(Department::sqlForFindingByPrimaryKey().empty());
    EXPECT_FALSE(Department::sqlForDeletingByPrimaryKey().empty());
    
    Department dept;
    bool needSelection;
    EXPECT_FALSE(dept.sqlForInserting(needSelection).empty());
} 