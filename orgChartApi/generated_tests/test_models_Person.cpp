#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/orm/DbClient.h>
#include "../models/Person.h"
#include "../models/Department.h"
#include "../models/Job.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;

class PersonTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(PersonTest, DefaultConstructor) {
    Person person;
    EXPECT_FALSE(person.getId());
    EXPECT_FALSE(person.getJobId());
    EXPECT_FALSE(person.getDepartmentId());
    EXPECT_FALSE(person.getManagerId());
    EXPECT_FALSE(person.getFirstName());
    EXPECT_FALSE(person.getLastName());
    EXPECT_FALSE(person.getHireDate());
}

TEST_F(PersonTest, SetAndGetId) {
    Person person;
    person.setId(1);
    ASSERT_TRUE(person.getId());
    EXPECT_EQ(person.getValueOfId(), 1);
}

TEST_F(PersonTest, SetAndGetJobId) {
    Person person;
    person.setJobId(2);
    ASSERT_TRUE(person.getJobId());
    EXPECT_EQ(person.getValueOfJobId(), 2);
}

TEST_F(PersonTest, SetAndGetDepartmentId) {
    Person person;
    person.setDepartmentId(3);
    ASSERT_TRUE(person.getDepartmentId());
    EXPECT_EQ(person.getValueOfDepartmentId(), 3);
}

TEST_F(PersonTest, SetAndGetManagerId) {
    Person person;
    person.setManagerId(4);
    ASSERT_TRUE(person.getManagerId());
    EXPECT_EQ(person.getValueOfManagerId(), 4);
}

TEST_F(PersonTest, SetAndGetFirstName) {
    Person person;
    person.setFirstName("John");
    ASSERT_TRUE(person.getFirstName());
    EXPECT_EQ(person.getValueOfFirstName(), "John");
}

TEST_F(PersonTest, SetAndGetLastName) {
    Person person;
    person.setLastName("Doe");
    ASSERT_TRUE(person.getLastName());
    EXPECT_EQ(person.getValueOfLastName(), "Doe");
}

TEST_F(PersonTest, SetAndGetHireDate) {
    Person person;
    trantor::Date hireDate = trantor::Date::now();
    person.setHireDate(hireDate);
    ASSERT_TRUE(person.getHireDate());
    EXPECT_EQ(person.getValueOfHireDate().toFormattedString(), hireDate.toFormattedString());
}

TEST_F(PersonTest, JsonConstruction) {
    Json::Value json;
    json["id"] = 1;
    json["job_id"] = 2;
    json["department_id"] = 3;
    json["manager_id"] = 4;
    json["first_name"] = "John";
    json["last_name"] = "Doe";
    json["hire_date"] = "2023-01-01";
    
    Person person(json);
    EXPECT_EQ(person.getValueOfId(), 1);
    EXPECT_EQ(person.getValueOfJobId(), 2);
    EXPECT_EQ(person.getValueOfDepartmentId(), 3);
    EXPECT_EQ(person.getValueOfManagerId(), 4);
    EXPECT_EQ(person.getValueOfFirstName(), "John");
    EXPECT_EQ(person.getValueOfLastName(), "Doe");
}

TEST_F(PersonTest, ToJson) {
    Person person;
    person.setId(1);
    person.setJobId(2);
    person.setDepartmentId(3);
    person.setManagerId(4);
    person.setFirstName("John");
    person.setLastName("Doe");
    trantor::Date hireDate = trantor::Date::fromDbString("2023-01-01");
    person.setHireDate(hireDate);
    
    Json::Value json = person.toJson();
    EXPECT_EQ(json["id"].asInt(), 1);
    EXPECT_EQ(json["job_id"].asInt(), 2);
    EXPECT_EQ(json["department_id"].asInt(), 3);
    EXPECT_EQ(json["manager_id"].asInt(), 4);
    EXPECT_EQ(json["first_name"].asString(), "John");
    EXPECT_EQ(json["last_name"].asString(), "Doe");
}

TEST_F(PersonTest, ValidateJsonForCreation_Valid) {
    Json::Value json;
    json["first_name"] = "John";
    json["last_name"] = "Doe";
    json["job_id"] = 1;
    json["department_id"] = 1;
    
    std::string err;
    EXPECT_TRUE(Person::validateJsonForCreation(json, err));
    EXPECT_TRUE(err.empty());
}

TEST_F(PersonTest, ValidateJsonForCreation_Invalid) {
    Json::Value json;
    // Missing required fields
    
    std::string err;
    EXPECT_FALSE(Person::validateJsonForCreation(json, err));
    EXPECT_FALSE(err.empty());
}

TEST_F(PersonTest, MasqueradedJson) {
    Person person;
    person.setId(1);
    person.setFirstName("John");
    person.setLastName("Doe");
    
    std::vector<std::string> masquerading = {"person_id", "first", "last"};
    Json::Value json = person.toMasqueradedJson(masquerading);
    
    EXPECT_EQ(json["person_id"].asInt(), 1);
    EXPECT_EQ(json["first"].asString(), "John");
    EXPECT_EQ(json["last"].asString(), "Doe");
}

TEST_F(PersonTest, TableMetadata) {
    EXPECT_EQ(Person::tableName, "persons");
    EXPECT_TRUE(Person::hasPrimaryKey);
    EXPECT_EQ(Person::primaryKeyName, "id");
    EXPECT_EQ(Person::getColumnNumber(), 7);
}

TEST_F(PersonTest, ColumnNames) {
    EXPECT_EQ(Person::Cols::_id, "id");
    EXPECT_EQ(Person::Cols::_job_id, "job_id");
    EXPECT_EQ(Person::Cols::_department_id, "department_id");
    EXPECT_EQ(Person::Cols::_manager_id, "manager_id");
    EXPECT_EQ(Person::Cols::_first_name, "first_name");
    EXPECT_EQ(Person::Cols::_last_name, "last_name");
    EXPECT_EQ(Person::Cols::_hire_date, "hire_date");
    
    EXPECT_EQ(Person::getColumnName(0), "id");
    EXPECT_EQ(Person::getColumnName(1), "job_id");
    EXPECT_EQ(Person::getColumnName(2), "department_id");
    EXPECT_EQ(Person::getColumnName(3), "manager_id");
    EXPECT_EQ(Person::getColumnName(4), "first_name");
    EXPECT_EQ(Person::getColumnName(5), "last_name");
    EXPECT_EQ(Person::getColumnName(6), "hire_date");
}

TEST_F(PersonTest, SqlQueries) {
    EXPECT_FALSE(Person::sqlForFindingByPrimaryKey().empty());
    EXPECT_FALSE(Person::sqlForDeletingByPrimaryKey().empty());
    
    Person person;
    bool needSelection;
    EXPECT_FALSE(person.sqlForInserting(needSelection).empty());
}

TEST_F(PersonTest, MoveStrings) {
    Person person;
    std::string firstName = "John";
    std::string lastName = "Doe";
    person.setFirstName(std::move(firstName));
    person.setLastName(std::move(lastName));
    
    EXPECT_EQ(person.getValueOfFirstName(), "John");
    EXPECT_EQ(person.getValueOfLastName(), "Doe");
    EXPECT_TRUE(firstName.empty()); // Original strings should be moved from
    EXPECT_TRUE(lastName.empty());
} 