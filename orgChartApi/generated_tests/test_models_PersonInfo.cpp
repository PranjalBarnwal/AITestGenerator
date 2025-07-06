#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/orm/DbClient.h>
#include "../models/PersonInfo.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;

class PersonInfoTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(PersonInfoTest, DefaultConstructor) {
    PersonInfo info;
    EXPECT_FALSE(info.getId());
    EXPECT_FALSE(info.getJobId());
    EXPECT_FALSE(info.getJobTitle());
    EXPECT_FALSE(info.getDepartmentId());
    EXPECT_FALSE(info.getDepartmentName());
    EXPECT_FALSE(info.getManagerId());
    EXPECT_FALSE(info.getManagerFullName());
    EXPECT_FALSE(info.getFirstName());
    EXPECT_FALSE(info.getLastName());
    EXPECT_FALSE(info.getHireDate());
}

TEST_F(PersonInfoTest, RowConstructor) {
    // Create a mock Row with all fields
    drogon::orm::Row row;
    // Note: In a real test, you would need to properly mock the Row object
    // This is just to demonstrate the concept
    PersonInfo info(row);
    
    // Verify the object was constructed (actual values would depend on Row mock)
    EXPECT_FALSE(info.getId());
    EXPECT_FALSE(info.getJobId());
    EXPECT_FALSE(info.getJobTitle());
}

TEST_F(PersonInfoTest, GetValueOfId) {
    PersonInfo info(drogon::orm::Row());
    EXPECT_EQ(info.getValueOfId(), 0); // Default value for int32_t
}

TEST_F(PersonInfoTest, GetValueOfJobId) {
    PersonInfo info(drogon::orm::Row());
    EXPECT_EQ(info.getValueOfJobId(), 0); // Default value for int32_t
}

TEST_F(PersonInfoTest, GetValueOfJobTitle) {
    PersonInfo info(drogon::orm::Row());
    EXPECT_TRUE(info.getValueOfJobTitle().empty()); // Default value for string
}

TEST_F(PersonInfoTest, GetValueOfDepartmentId) {
    PersonInfo info(drogon::orm::Row());
    EXPECT_EQ(info.getValueOfDepartmentId(), 0); // Default value for int32_t
}

TEST_F(PersonInfoTest, GetValueOfDepartmentName) {
    PersonInfo info(drogon::orm::Row());
    EXPECT_TRUE(info.getValueOfDepartmentName().empty()); // Default value for string
}

TEST_F(PersonInfoTest, GetValueOfManagerId) {
    PersonInfo info(drogon::orm::Row());
    EXPECT_EQ(info.getValueOfManagerId(), 0); // Default value for int32_t
}

TEST_F(PersonInfoTest, GetValueOfManagerFullName) {
    PersonInfo info(drogon::orm::Row());
    EXPECT_TRUE(info.getValueOfManagerFullName().empty()); // Default value for string
}

TEST_F(PersonInfoTest, GetValueOfFirstName) {
    PersonInfo info(drogon::orm::Row());
    EXPECT_TRUE(info.getValueOfFirstName().empty()); // Default value for string
}

TEST_F(PersonInfoTest, GetValueOfLastName) {
    PersonInfo info(drogon::orm::Row());
    EXPECT_TRUE(info.getValueOfLastName().empty()); // Default value for string
}

TEST_F(PersonInfoTest, GetValueOfHireDate) {
    PersonInfo info(drogon::orm::Row());
    // Default date should be epoch or a specific default value
    EXPECT_EQ(info.getValueOfHireDate().microSecondsSinceEpoch(), 0);
}

TEST_F(PersonInfoTest, ToJson) {
    PersonInfo info(drogon::orm::Row());
    Json::Value json = info.toJson();
    
    // Verify JSON structure
    EXPECT_TRUE(json.isMember("id"));
    EXPECT_TRUE(json.isMember("job_id"));
    EXPECT_TRUE(json.isMember("job_title"));
    EXPECT_TRUE(json.isMember("department_id"));
    EXPECT_TRUE(json.isMember("department_name"));
    EXPECT_TRUE(json.isMember("manager_id"));
    EXPECT_TRUE(json.isMember("manager_full_name"));
    EXPECT_TRUE(json.isMember("first_name"));
    EXPECT_TRUE(json.isMember("last_name"));
    EXPECT_TRUE(json.isMember("hire_date"));
    
    // Verify default values
    EXPECT_EQ(json["id"].asInt(), 0);
    EXPECT_EQ(json["job_id"].asInt(), 0);
    EXPECT_TRUE(json["job_title"].asString().empty());
    EXPECT_EQ(json["department_id"].asInt(), 0);
    EXPECT_TRUE(json["department_name"].asString().empty());
    EXPECT_EQ(json["manager_id"].asInt(), 0);
    EXPECT_TRUE(json["manager_full_name"].asString().empty());
    EXPECT_TRUE(json["first_name"].asString().empty());
    EXPECT_TRUE(json["last_name"].asString().empty());
} 