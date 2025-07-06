// File: test_DepartmentsController.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/app.h>
#include "../DepartmentsController.h"
#include "../models/Department.h"
#include "../models/Person.h"
#include "../utils/utils.h"
#include <memory>
#include <string>
#include <vector>

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

class MockDbClient : public drogon::orm::DbClient {
public:
    MOCK_METHOD2(execSqlAsync, void(const std::string&, std::function<void(const drogon::orm::Result&)>&&));
    MOCK_METHOD3(execSqlAsync, void(const std::string&, std::function<void(const drogon::orm::Result&)>&&, std::function<void(const std::exception_ptr&)>&&));
    MOCK_METHOD2(newTransactionAsync, void(std::function<void(std::shared_ptr<drogon::orm::Transaction>)>&&, std::function<void(const std::exception_ptr&)>&&));
    MOCK_METHOD0(isInTransaction, bool());
    MOCK_METHOD0(hasAvailableConnections, bool());
    MOCK_METHOD0(getIdleConnectionCount, size_t());
    MOCK_METHOD0(getConnectionNum, size_t());
    MOCK_METHOD0(getBusyConnectionCount, size_t());
};

class DepartmentsControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockDbClient = std::make_shared<MockDbClient>();
        drogon::app().registerSyncAdvice([this](const std::string& name) -> std::shared_ptr<drogon::orm::DbClient> {
            if (name == "default") return mockDbClient;
            return nullptr;
        });
        controller = std::make_shared<DepartmentsController>();
    }

    void TearDown() override {
        drogon::app().registerSyncAdvice(nullptr);
    }

    std::shared_ptr<MockDbClient> mockDbClient;
    std::shared_ptr<DepartmentsController> controller;
};

TEST_F(DepartmentsControllerTest, GetDepartments_Success) {
    auto req = HttpRequest::newHttpRequest();
    req->setParameter("offset", "0");
    req->setParameter("limit", "10");
    req->setParameter("sort_field", "id");
    req->setParameter("sort_order", "asc");

    std::vector<Department> mockDepartments;
    mockDepartments.emplace_back(Json::Value());
    mockDepartments.back().setId(1);
    mockDepartments.back().setName("Test Dept");

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([&mockDepartments](const std::string&, std::function<void(const drogon::orm::Result&)>&& callback, std::function<void(const std::exception_ptr&)>&&) {
            drogon::orm::Result result(nullptr);
            callback(result);
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        auto json = resp->getJsonObject();
        EXPECT_TRUE(json);
        EXPECT_EQ(json->size(), 1);
    };

    controller->get(req, callback);
}

TEST_F(DepartmentsControllerTest, GetDepartments_DatabaseError) {
    auto req = HttpRequest::newHttpRequest();
    req->setParameter("offset", "0");
    req->setParameter("limit", "10");

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string&, std::function<void(const drogon::orm::Result&)>&&, std::function<void(const std::exception_ptr&)>&& errorCallback) {
            errorCallback(std::make_exception_ptr(DrogonDbException("Database error")));
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
        auto json = resp->getJsonObject();
        EXPECT_TRUE(json);
        EXPECT_EQ(json->get("error", "").asString(), "database error");
    };

    controller->get(req, callback);
}

TEST_F(DepartmentsControllerTest, GetOneDepartment_Success) {
    auto req = HttpRequest::newHttpRequest();
    int departmentId = 1;

    Department mockDepartment(Json::Value());
    mockDepartment.setId(departmentId);
    mockDepartment.setName("Test Dept");

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([&mockDepartment](const std::string&, std::function<void(const drogon::orm::Result&)>&& callback, std::function<void(const std::exception_ptr&)>&&) {
            drogon::orm::Result result(nullptr);
            callback(result);
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
        auto json = resp->getJsonObject();
        EXPECT_TRUE(json);
    };

    controller->getOne(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, GetOneDepartment_NotFound) {
    auto req = HttpRequest::newHttpRequest();
    int departmentId = 999;

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string&, std::function<void(const drogon::orm::Result&)>&&, std::function<void(const std::exception_ptr&)>&& errorCallback) {
            errorCallback(std::make_exception_ptr(drogon::orm::UnexpectedRows("Not found")));
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k404NotFound);
    };

    controller->getOne(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, CreateOneDepartment_Success) {
    auto req = HttpRequest::newHttpRequest();
    Department newDept(Json::Value());
    newDept.setName("New Dept");

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([&newDept](const std::string&, std::function<void(const drogon::orm::Result&)>&& callback, std::function<void(const std::exception_ptr&)>&&) {
            drogon::orm::Result result(nullptr);
            callback(result);
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
        auto json = resp->getJsonObject();
        EXPECT_TRUE(json);
    };

    controller->createOne(req, callback, std::move(newDept));
}

TEST_F(DepartmentsControllerTest, CreateOneDepartment_DatabaseError) {
    auto req = HttpRequest::newHttpRequest();
    Department newDept(Json::Value());
    newDept.setName("New Dept");

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string&, std::function<void(const drogon::orm::Result&)>&&, std::function<void(const std::exception_ptr&)>&& errorCallback) {
            errorCallback(std::make_exception_ptr(DrogonDbException("Database error")));
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
        auto json = resp->getJsonObject();
        EXPECT_TRUE(json);
        EXPECT_EQ(json->get("error", "").asString(), "database error");
    };

    controller->createOne(req, callback, std::move(newDept));
}

TEST_F(DepartmentsControllerTest, UpdateOneDepartment_Success) {
    auto req = HttpRequest::newHttpRequest();
    int departmentId = 1;
    Department updateDept(Json::Value());
    updateDept.setName("Updated Dept");

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .Times(2)
        .WillRepeatedly(Invoke([](const std::string&, std::function<void(const drogon::orm::Result&)>&& callback, std::function<void(const std::exception_ptr&)>&&) {
            drogon::orm::Result result(nullptr);
            callback(result);
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k204NoContent);
    };

    controller->updateOne(req, callback, departmentId, std::move(updateDept));
}

TEST_F(DepartmentsControllerTest, UpdateOneDepartment_NotFound) {
    auto req = HttpRequest::newHttpRequest();
    int departmentId = 999;
    Department updateDept(Json::Value());
    updateDept.setName("Updated Dept");

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string&, std::function<void(const drogon::orm::Result&)>&&, std::function<void(const std::exception_ptr&)>&& errorCallback) {
            errorCallback(std::make_exception_ptr(DrogonDbException("Not found")));
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k404NotFound);
    };

    controller->updateOne(req, callback, departmentId, std::move(updateDept));
}

TEST_F(DepartmentsControllerTest, DeleteOneDepartment_Success) {
    auto req = HttpRequest::newHttpRequest();
    int departmentId = 1;

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string&, std::function<void(const drogon::orm::Result&)>&& callback, std::function<void(const std::exception_ptr&)>&&) {
            drogon::orm::Result result(nullptr);
            callback(result);
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k204NoContent);
    };

    controller->deleteOne(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, DeleteOneDepartment_DatabaseError) {
    auto req = HttpRequest::newHttpRequest();
    int departmentId = 1;

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string&, std::function<void(const drogon::orm::Result&)>&&, std::function<void(const std::exception_ptr&)>&& errorCallback) {
            errorCallback(std::make_exception_ptr(DrogonDbException("Database error")));
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
    };

    controller->deleteOne(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, GetDepartmentPersons_Success) {
    auto req = HttpRequest::newHttpRequest();
    int departmentId = 1;

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .Times(2)
        .WillRepeatedly(Invoke([](const std::string&, std::function<void(const drogon::orm::Result&)>&& callback, std::function<void(const std::exception_ptr&)>&&) {
            drogon::orm::Result result(nullptr);
            callback(result);
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        auto json = resp->getJsonObject();
        EXPECT_TRUE(json);
    };

    controller->getDepartmentPersons(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, GetDepartmentPersons_NotFound) {
    auto req = HttpRequest::newHttpRequest();
    int departmentId = 999;

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _))
        .WillOnce(Invoke([](const std::string&, std::function<void(const drogon::orm::Result&)>&&, std::function<void(const std::exception_ptr&)>&& errorCallback) {
            errorCallback(std::make_exception_ptr(DrogonDbException("Not found")));
        }));

    auto callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k404NotFound);
    };

    controller->getDepartmentPersons(req, callback, departmentId);
}