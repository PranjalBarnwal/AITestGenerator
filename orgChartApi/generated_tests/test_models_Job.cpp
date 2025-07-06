#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/orm/DbClient.h>
#include "../models/Job.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;

class JobTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(JobTest, DefaultConstructor) {
    Job job;
    EXPECT_FALSE(job.getId());  // Should be null/empty
    EXPECT_FALSE(job.getTitle()); // Should be null/empty
}

TEST_F(JobTest, SetAndGetId) {
    Job job;
    job.setId(1);
    ASSERT_TRUE(job.getId());
    EXPECT_EQ(job.getValueOfId(), 1);
}

TEST_F(JobTest, SetAndGetTitle) {
    Job job;
    job.setTitle("Software Engineer");
    ASSERT_TRUE(job.getTitle());
    EXPECT_EQ(job.getValueOfTitle(), "Software Engineer");
}

TEST_F(JobTest, JsonConstruction) {
    Json::Value json;
    json["id"] = 1;
    json["title"] = "Software Engineer";
    
    Job job(json);
    EXPECT_EQ(job.getValueOfId(), 1);
    EXPECT_EQ(job.getValueOfTitle(), "Software Engineer");
}

TEST_F(JobTest, ToJson) {
    Job job;
    job.setId(1);
    job.setTitle("Software Engineer");
    
    Json::Value json = job.toJson();
    EXPECT_EQ(json["id"].asInt(), 1);
    EXPECT_EQ(json["title"].asString(), "Software Engineer");
}

TEST_F(JobTest, ValidateJsonForCreation_Valid) {
    Json::Value json;
    json["title"] = "Software Engineer";
    
    std::string err;
    EXPECT_TRUE(Job::validateJsonForCreation(json, err));
    EXPECT_TRUE(err.empty());
}

TEST_F(JobTest, ValidateJsonForCreation_Invalid) {
    Json::Value json;
    // Missing required title field
    
    std::string err;
    EXPECT_FALSE(Job::validateJsonForCreation(json, err));
    EXPECT_FALSE(err.empty());
}

TEST_F(JobTest, ValidateJsonForUpdate_Valid) {
    Json::Value json;
    json["title"] = "Senior Software Engineer";
    
    std::string err;
    EXPECT_TRUE(Job::validateJsonForUpdate(json, err));
    EXPECT_TRUE(err.empty());
}

TEST_F(JobTest, MasqueradedJson) {
    Job job;
    job.setId(1);
    job.setTitle("Software Engineer");
    
    std::vector<std::string> masquerading = {"job_id", "job_title"};
    Json::Value json = job.toMasqueradedJson(masquerading);
    
    EXPECT_EQ(json["job_id"].asInt(), 1);
    EXPECT_EQ(json["job_title"].asString(), "Software Engineer");
}

TEST_F(JobTest, TableMetadata) {
    EXPECT_EQ(Job::tableName, "jobs");
    EXPECT_TRUE(Job::hasPrimaryKey);
    EXPECT_EQ(Job::primaryKeyName, "id");
    EXPECT_EQ(Job::getColumnNumber(), 2);
}

TEST_F(JobTest, ColumnNames) {
    EXPECT_EQ(Job::Cols::_id, "id");
    EXPECT_EQ(Job::Cols::_title, "title");
    
    EXPECT_EQ(Job::getColumnName(0), "id");
    EXPECT_EQ(Job::getColumnName(1), "title");
}

TEST_F(JobTest, SqlQueries) {
    EXPECT_FALSE(Job::sqlForFindingByPrimaryKey().empty());
    EXPECT_FALSE(Job::sqlForDeletingByPrimaryKey().empty());
    
    Job job;
    bool needSelection;
    EXPECT_FALSE(job.sqlForInserting(needSelection).empty());
}

TEST_F(JobTest, MoveTitle) {
    Job job;
    std::string title = "Software Engineer";
    job.setTitle(std::move(title));
    EXPECT_EQ(job.getValueOfTitle(), "Software Engineer");
    EXPECT_TRUE(title.empty()); // Original string should be moved from
} 