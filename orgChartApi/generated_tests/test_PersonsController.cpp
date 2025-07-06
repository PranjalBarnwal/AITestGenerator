// File: test_PersonsController.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/app.h>
#include "../PersonsController.h"
#include "../models/Person.h"
#include "../models/PersonInfo.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon_model::org_chart;
using namespace testing;

class MockDbClient : public drogon::orm::DbClient {
public:
    MOCK_METHOD(void, execSqlAsync, (const std::string&, std::function<void(const drogon::orm::Result&)>, std::function<void(const drogon::orm::DrogonDbException&)>), (override));
    MOCK_METHOD(void, execSqlAsync, (const std::string&, const std::vector<std::string>&, std::function<void(const drogon::orm::Result&)>, std::function<void(const drogon::orm::DrogonDbException&)>), (override));
    MOCK_METHOD(std::shared_ptr<drogon::orm::Transaction>, newTransaction, (), (override));
    MOCK_METHOD(void, newTransactionAsync, (std::function<void(std::shared_ptr<drogon::orm::Transaction>)>, std::function<void(const drogon::orm::DrogonDbException&)>), (override));
};

class PersonsControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        app().registerSyncAdvice([](const HttpRequestPtr& req) -> HttpResponsePtr {
            return HttpResponse::newHttpResponse();
        });
    }

    HttpRequestPtr createRequest(const std::string& method, const std::string& path, const Json::Value& body = Json::Value()) {
        auto req = HttpRequest::newHttpRequest();
        req->setMethod(method == "GET" ? Get : method == "POST" ? Post : method == "PUT" ? Put : Delete);
        req->setPath(path);
        if (!body.isNull()) {
            req->setBody(body.toStyledString());
        }
        return req;
    }

    std::shared_ptr<MockDbClient> mockDbClient = std::make_shared<MockDbClient>();
    PersonsController controller;
};

TEST_F(PersonsControllerTest, GetPersons_Success) {
    auto req = createRequest("GET", "/persons");
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, std::function<void(const drogon::orm::Result&)> resultCallback, auto) {
            drogon::orm::Result result;
            result.push_back(drogon::orm::Row());
            resultCallback(result);
        }));

    controller.get(req, std::move(callback));
}

TEST_F(PersonsControllerTest, GetPersons_EmptyResult) {
    auto req = createRequest("GET", "/persons");
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k404NotFound);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, std::function<void(const drogon::orm::Result&)> resultCallback, auto) {
            drogon::orm::Result result;
            resultCallback(result);
        }));

    controller.get(req, std::move(callback));
}

TEST_F(PersonsControllerTest, GetPersons_DbError) {
    auto req = createRequest("GET", "/persons");
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, auto, std::function<void(const drogon::orm::DrogonDbException&)> errorCallback) {
            errorCallback(drogon::orm::DrogonDbException("DB Error"));
        }));

    controller.get(req, std::move(callback));
}

TEST_F(PersonsControllerTest, GetOnePerson_Success) {
    auto req = createRequest("GET", "/persons/1");
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, std::function<void(const drogon::orm::Result&)> resultCallback, auto) {
            drogon::orm::Result result;
            result.push_back(drogon::orm::Row());
            resultCallback(result);
        }));

    controller.getOne(req, std::move(callback), 1);
}

TEST_F(PersonsControllerTest, GetOnePerson_NotFound) {
    auto req = createRequest("GET", "/persons/1");
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k404NotFound);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, std::function<void(const drogon::orm::Result&)> resultCallback, auto) {
            drogon::orm::Result result;
            resultCallback(result);
        }));

    controller.getOne(req, std::move(callback), 1);
}

TEST_F(PersonsControllerTest, CreateOnePerson_Success) {
    Json::Value body;
    body["first_name"] = "John";
    body["last_name"] = "Doe";
    auto req = createRequest("POST", "/persons", body);
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, std::function<void(const drogon::orm::Result&)> resultCallback, auto) {
            drogon::orm::Result result;
            resultCallback(result);
        }));

    Person person;
    controller.createOne(req, std::move(callback), std::move(person));
}

TEST_F(PersonsControllerTest, CreateOnePerson_DbError) {
    Json::Value body;
    body["first_name"] = "John";
    body["last_name"] = "Doe";
    auto req = createRequest("POST", "/persons", body);
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, auto, std::function<void(const drogon::orm::DrogonDbException&)> errorCallback) {
            errorCallback(drogon::orm::DrogonDbException("DB Error"));
        }));

    Person person;
    controller.createOne(req, std::move(callback), std::move(person));
}

TEST_F(PersonsControllerTest, UpdateOnePerson_Success) {
    Json::Value body;
    body["first_name"] = "John";
    auto req = createRequest("PUT", "/persons/1", body);
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k204NoContent);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, std::function<void(const drogon::orm::Result&)> resultCallback, auto) {
            drogon::orm::Result result;
            resultCallback(result);
        }));

    Person person;
    controller.updateOne(req, std::move(callback), 1, std::move(person));
}

TEST_F(PersonsControllerTest, UpdateOnePerson_NotFound) {
    Json::Value body;
    body["first_name"] = "John";
    auto req = createRequest("PUT", "/persons/1", body);
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k404NotFound);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, auto, std::function<void(const drogon::orm::DrogonDbException&)> errorCallback) {
            errorCallback(drogon::orm::DrogonDbException("Not Found"));
        }));

    Person person;
    controller.updateOne(req, std::move(callback), 1, std::move(person));
}

TEST_F(PersonsControllerTest, DeleteOnePerson_Success) {
    auto req = createRequest("DELETE", "/persons/1");
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k204NoContent);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, std::function<void(const drogon::orm::Result&)> resultCallback, auto) {
            drogon::orm::Result result;
            resultCallback(result);
        }));

    controller.deleteOne(req, std::move(callback), 1);
}

TEST_F(PersonsControllerTest, DeleteOnePerson_DbError) {
    auto req = createRequest("DELETE", "/persons/1");
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k500InternalServerError);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, auto, std::function<void(const drogon::orm::DrogonDbException&)> errorCallback) {
            errorCallback(drogon::orm::DrogonDbException("DB Error"));
        }));

    controller.deleteOne(req, std::move(callback), 1);
}

TEST_F(PersonsControllerTest, GetDirectReports_Success) {
    auto req = createRequest("GET", "/persons/1/reports");
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, std::function<void(const drogon::orm::Result&)> resultCallback, auto) {
            drogon::orm::Result result;
            result.push_back(drogon::orm::Row());
            resultCallback(result);
        }));

    controller.getDirectReports(req, std::move(callback), 1);
}

TEST_F(PersonsControllerTest, GetDirectReports_NotFound) {
    auto req = createRequest("GET", "/persons/1/reports");
    std::function<void(const HttpResponsePtr&)> callback = [](const HttpResponsePtr& resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k404NotFound);
    };

    EXPECT_CALL(*mockDbClient, execSqlAsync(_, _, _, _))
        .WillOnce(Invoke([](const std::string&, const std::vector<std::string>&, auto, std::function<void(const drogon::orm::DrogonDbException&)> errorCallback) {
            errorCallback(drogon::orm::DrogonDbException("Not Found"));
        }));

    controller.getDirectReports(req, std::move(callback), 1);
}