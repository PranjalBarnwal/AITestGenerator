// File: test_JobsController.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/app.h>
#include "../JobsController.h"
#include "../models/Job.h"
#include "../models/Person.h"
#include "../utils/utils.h"
#include <memory>
#include <string>
#include <vector>

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;

class MockDbClient : public drogon::orm::DbClient {
public:
    MOCK_METHOD2(execSqlAsync, void(const std::string &, std::function<void(const drogon::orm::Result &)> &&));
    MOCK_METHOD3(execSqlAsync, void(const std::string &, std::function<void(const drogon::orm::Result &)> &&, std::function<void(const std::exception_ptr &)> &&));
    MOCK_METHOD1(newTransactionAsync, void(std::function<void(const std::shared_ptr<drogon::orm::Transaction> &)> &&));
    MOCK_METHOD0(isInTransaction, bool());
    MOCK_METHOD0(hasAvailableConnections, bool());
    MOCK_CONST_METHOD0(type, drogon::orm::ClientType());
};

class JobsControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        app().registerSyncAdvice([](const HttpRequestPtr &req) { return req; });
        mockDbClient = std::make_shared<MockDbClient>();
        drogon::app().registerDbClient(mockDbClient);
    }

    void TearDown() override {
        drogon::app().clearDbClients();
    }

    std::shared_ptr<MockDbClient> mockDbClient;
    JobsController controller;
};

TEST_F(JobsControllerTest, GetJobs_Success) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Get);
    req->setPath("/jobs");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&cb, std::function<void(const std::exception_ptr &)> &&) {
            drogon::orm::Result res(nullptr);
            cb(res);
        }));

    controller.get(req, std::move(callback));
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, GetJobs_DatabaseError) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Get);
    req->setPath("/jobs");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&, std::function<void(const std::exception_ptr &)> &&errCb) {
            errCb(std::make_exception_ptr(std::runtime_error("DB Error")));
        }));

    controller.get(req, std::move(callback));
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, GetOneJob_Success) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Get);
    req->setPath("/jobs/1");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&cb, std::function<void(const std::exception_ptr &)> &&) {
            drogon::orm::Result res(nullptr);
            cb(res);
        }));

    controller.getOne(req, std::move(callback), 1);
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, GetOneJob_NotFound) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Get);
    req->setPath("/jobs/1");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k404NotFound);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&, std::function<void(const std::exception_ptr &)> &&errCb) {
            errCb(std::make_exception_ptr(drogon::orm::UnexpectedRows("Not found")));
        }));

    controller.getOne(req, std::move(callback), 1);
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, CreateOneJob_Success) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath("/jobs");
    Job job;
    job.setTitle("Test Job");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&cb, std::function<void(const std::exception_ptr &)> &&) {
            drogon::orm::Result res(nullptr);
            cb(res);
        }));

    controller.createOne(req, std::move(callback), std::move(job));
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, CreateOneJob_DatabaseError) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath("/jobs");
    Job job;
    job.setTitle("Test Job");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&, std::function<void(const std::exception_ptr &)> &&errCb) {
            errCb(std::make_exception_ptr(std::runtime_error("DB Error")));
        }));

    controller.createOne(req, std::move(callback), std::move(job));
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, UpdateOneJob_Success) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Put);
    req->setPath("/jobs/1");
    req->setBody("{\"title\": \"Updated Job\"}");
    Job jobDetails;
    jobDetails.setTitle("Updated Job");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k204NoContent);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .Times(2)
        .WillRepeatedly(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&cb, std::function<void(const std::exception_ptr &)> &&) {
            drogon::orm::Result res(nullptr);
            cb(res);
        }));

    controller.updateOne(req, std::move(callback), 1, std::move(jobDetails));
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, UpdateOneJob_NotFound) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Put);
    req->setPath("/jobs/1");
    req->setBody("{\"title\": \"Updated Job\"}");
    Job jobDetails;
    jobDetails.setTitle("Updated Job");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k404NotFound);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&, std::function<void(const std::exception_ptr &)> &&errCb) {
            errCb(std::make_exception_ptr(drogon::orm::UnexpectedRows("Not found")));
        }));

    controller.updateOne(req, std::move(callback), 1, std::move(jobDetails));
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, DeleteOneJob_Success) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Delete);
    req->setPath("/jobs/1");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k204NoContent);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&cb, std::function<void(const std::exception_ptr &)> &&) {
            drogon::orm::Result res(nullptr);
            cb(res);
        }));

    controller.deleteOne(req, std::move(callback), 1);
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, DeleteOneJob_DatabaseError) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Delete);
    req->setPath("/jobs/1");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&, std::function<void(const std::exception_ptr &)> &&errCb) {
            errCb(std::make_exception_ptr(std::runtime_error("DB Error")));
        }));

    controller.deleteOne(req, std::move(callback), 1);
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, GetJobPersons_Success) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Get);
    req->setPath("/jobs/1/persons");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .Times(2)
        .WillRepeatedly(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&cb, std::function<void(const std::exception_ptr &)> &&) {
            drogon::orm::Result res(nullptr);
            cb(res);
        }));

    controller.getJobPersons(req, std::move(callback), 1);
    EXPECT_TRUE(callbackCalled);
}

TEST_F(JobsControllerTest, GetJobPersons_JobNotFound) {
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Get);
    req->setPath("/jobs/1/persons");

    bool callbackCalled = false;
    auto callback = [&callbackCalled](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k404NotFound);
        callbackCalled = true;
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string &, std::function<void(const drogon::orm::Result &)> &&, std::function<void(const std::exception_ptr &)> &&errCb) {
            errCb(std::make_exception_ptr(drogon::orm::UnexpectedRows("Not found")));
        }));

    controller.getJobPersons(req, std::move(callback), 1);
    EXPECT_TRUE(callbackCalled);
}