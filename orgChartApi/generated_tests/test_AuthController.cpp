// File: test_AuthController.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/app.h>
#include <drogon/orm/Mapper.h>
#include <third_party/libbcrypt/include/bcrypt/BCrypt.hpp>
#include "../AuthController.h"
#include "../plugins/JwtPlugin.h"
#include "../models/User.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;
using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

class MockJwtPlugin : public JwtPlugin {
public:
    MOCK_METHOD0(init, JwtPlugin::Jwt&());
    MOCK_METHOD2(encode, std::string(const std::string&, const std::string&));
};

class MockDbClient : public drogon::orm::DbClient {
public:
    MOCK_METHOD1(execSqlAsyncFuture, std::future<drogon::orm::Result>(const std::string&));
    MOCK_METHOD2(execSqlAsyncFuture, std::future<drogon::orm::Result>(const std::string&, const std::vector<std::string>&));
    MOCK_METHOD3(execSqlAsyncFuture, std::future<drogon::orm::Result>(const std::string&, const std::vector<std::string>&, const std::vector<int>&));
    MOCK_METHOD2(execSqlSync, drogon::orm::Result(const std::string&, const std::vector<std::string>&));
    MOCK_METHOD3(execSqlSync, drogon::orm::Result(const std::string&, const std::vector<std::string>&, const std::vector<int>&));
    MOCK_METHOD1(execSqlSync, drogon::orm::Result(const std::string&));
    MOCK_METHOD0(newTransaction, std::shared_ptr<drogon::orm::Transaction>());
    MOCK_METHOD1(newTransactionAsync, void(const std::function<void(std::shared_ptr<drogon::orm::Transaction>)>&));
    MOCK_CONST_METHOD0(type, drogon::orm::ClientType());
    MOCK_CONST_METHOD0(hasAvailableConnections, bool());
};

class AuthControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        app().registerPlugin(&mockJwtPlugin);
        ON_CALL(mockJwtPlugin, init()).WillByDefault(Return(jwt));
        ON_CALL(mockJwtPlugin, encode(_, _)).WillByDefault(Return("mocked_token"));
    }

    void TearDown() override {
        app().removePlugin("JwtPlugin");
    }

    MockJwtPlugin mockJwtPlugin;
    JwtPlugin::Jwt jwt;
    AuthController controller;
};

TEST_F(AuthControllerTest, RegisterUser_MissingFields) {
    User user;
    auto req = HttpRequest::newHttpRequest();
    std::shared_ptr<HttpResponse> response;
    auto callback = [&response](const HttpResponsePtr &resp) { response = resp; };

    controller.registerUser(req, std::move(callback), std::move(user));

    ASSERT_EQ(response->getStatusCode(), HttpStatusCode::k400BadRequest);
    auto json = response->getJsonObject();
    EXPECT_EQ((*json)["error"].asString(), "missing fields");
}

TEST_F(AuthControllerTest, RegisterUser_UsernameTaken) {
    User user;
    user.setUsername("testuser");
    user.setPassword("password");
    auto req = HttpRequest::newHttpRequest();
    std::shared_ptr<HttpResponse> response;
    auto callback = [&response](const HttpResponsePtr &resp) { response = resp; };

    NiceMock<MockDbClient> mockDbClient;
    app().registerSyncDbClient(std::shared_ptr<DbClient>(&mockDbClient, [](DbClient*) {}));

    controller.registerUser(req, std::move(callback), std::move(user));

    ASSERT_EQ(response->getStatusCode(), HttpStatusCode::k400BadRequest);
    auto json = response->getJsonObject();
    EXPECT_EQ((*json)["error"].asString(), "username is taken");
}

TEST_F(AuthControllerTest, RegisterUser_Success) {
    User user;
    user.setUsername("newuser");
    user.setPassword("password");
    auto req = HttpRequest::newHttpRequest();
    std::shared_ptr<HttpResponse> response;
    auto callback = [&response](const HttpResponsePtr &resp) { response = resp; };

    NiceMock<MockDbClient> mockDbClient;
    app().registerSyncDbClient(std::shared_ptr<DbClient>(&mockDbClient, [](DbClient*) {}));

    controller.registerUser(req, std::move(callback), std::move(user));

    ASSERT_EQ(response->getStatusCode(), HttpStatusCode::k201Created);
    auto json = response->getJsonObject();
    EXPECT_EQ((*json)["username"].asString(), "newuser");
    EXPECT_EQ((*json)["token"].asString(), "mocked_token");
}

TEST_F(AuthControllerTest, RegisterUser_DatabaseError) {
    User user;
    user.setUsername("newuser");
    user.setPassword("password");
    auto req = HttpRequest::newHttpRequest();
    std::shared_ptr<HttpResponse> response;
    auto callback = [&response](const HttpResponsePtr &resp) { response = resp; };

    NiceMock<MockDbClient> mockDbClient;
    app().registerSyncDbClient(std::shared_ptr<DbClient>(&mockDbClient, [](DbClient*) {}));
    ON_CALL(mockDbClient, execSqlSync(_)).WillByDefault(Throw(DrogonDbException("DB error")));

    controller.registerUser(req, std::move(callback), std::move(user));

    ASSERT_EQ(response->getStatusCode(), HttpStatusCode::k500InternalServerError);
    auto json = response->getJsonObject();
    EXPECT_EQ((*json)["error"].asString(), "database error");
}

TEST_F(AuthControllerTest, LoginUser_MissingFields) {
    User user;
    auto req = HttpRequest::newHttpRequest();
    std::shared_ptr<HttpResponse> response;
    auto callback = [&response](const HttpResponsePtr &resp) { response = resp; };

    controller.loginUser(req, std::move(callback), std::move(user));

    ASSERT_EQ(response->getStatusCode(), HttpStatusCode::k400BadRequest);
    auto json = response->getJsonObject();
    EXPECT_EQ((*json)["error"].asString(), "missing fields");
}

TEST_F(AuthControllerTest, LoginUser_UserNotFound) {
    User user;
    user.setUsername("unknown");
    user.setPassword("password");
    auto req = HttpRequest::newHttpRequest();
    std::shared_ptr<HttpResponse> response;
    auto callback = [&response](const HttpResponsePtr &resp) { response = resp; };

    NiceMock<MockDbClient> mockDbClient;
    app().registerSyncDbClient(std::shared_ptr<DbClient>(&mockDbClient, [](DbClient*) {}));

    controller.loginUser(req, std::move(callback), std::move(user));

    ASSERT_EQ(response->getStatusCode(), HttpStatusCode::k400BadRequest);
    auto json = response->getJsonObject();
    EXPECT_EQ((*json)["error"].asString(), "user not found");
}

TEST_F(AuthControllerTest, LoginUser_InvalidPassword) {
    User user;
    user.setUsername("testuser");
    user.setPassword("wrongpassword");
    auto req = HttpRequest::newHttpRequest();
    std::shared_ptr<HttpResponse> response;
    auto callback = [&response](const HttpResponsePtr &resp) { response = resp; };

    NiceMock<MockDbClient> mockDbClient;
    app().registerSyncDbClient(std::shared_ptr<DbClient>(&mockDbClient, [](DbClient*) {}));

    controller.loginUser(req, std::move(callback), std::move(user));

    ASSERT_EQ(response->getStatusCode(), HttpStatusCode::k401Unauthorized);
    auto json = response->getJsonObject();
    EXPECT_EQ((*json)["error"].asString(), "username and password do not match");
}

TEST_F(AuthControllerTest, LoginUser_Success) {
    User user;
    user.setUsername("testuser");
    user.setPassword("correctpassword");
    auto req = HttpRequest::newHttpRequest();
    std::shared_ptr<HttpResponse> response;
    auto callback = [&response](const HttpResponsePtr &resp) { response = resp; };

    NiceMock<MockDbClient> mockDbClient;
    app().registerSyncDbClient(std::shared_ptr<DbClient>(&mockDbClient, [](DbClient*) {}));

    controller.loginUser(req, std::move(callback), std::move(user));

    ASSERT_EQ(response->getStatusCode(), HttpStatusCode::k200OK);
    auto json = response->getJsonObject();
    EXPECT_EQ((*json)["username"].asString(), "testuser");
    EXPECT_EQ((*json)["token"].asString(), "mocked_token");
}

TEST_F(AuthControllerTest, LoginUser_DatabaseError) {
    User user;
    user.setUsername("testuser");
    user.setPassword("password");
    auto req = HttpRequest::newHttpRequest();
    std::shared_ptr<HttpResponse> response;
    auto callback = [&response](const HttpResponsePtr &resp) { response = resp; };

    NiceMock<MockDbClient> mockDbClient;
    app().registerSyncDbClient(std::shared_ptr<DbClient>(&mockDbClient, [](DbClient*) {}));
    ON_CALL(mockDbClient, execSqlSync(_)).WillByDefault(Throw(DrogonDbException("DB error")));

    controller.loginUser(req, std::move(callback), std::move(user));

    ASSERT_EQ(response->getStatusCode(), HttpStatusCode::k500InternalServerError);
    auto json = response->getJsonObject();
    EXPECT_EQ((*json)["error"].asString(), "database error");
}

TEST_F(AuthControllerTest, AreFieldsValid_ValidUser) {
    User user;
    user.setUsername("testuser");
    user.setPassword("password");

    EXPECT_TRUE(controller.areFieldsValid(user));
}

TEST_F(AuthControllerTest, AreFieldsValid_InvalidUser) {
    User user;

    EXPECT_FALSE(controller.areFieldsValid(user));
}

TEST_F(AuthControllerTest, IsPasswordValid_ValidPassword) {
    std::string password = "testpass";
    std::string hash = BCrypt::generateHash(password);

    EXPECT_TRUE(controller.isPasswordValid(password, hash));
}

TEST_F(AuthControllerTest, IsPasswordValid_InvalidPassword) {
    std::string password = "testpass";
    std::string hash = BCrypt::generateHash("wrongpass");

    EXPECT_FALSE(controller.isPasswordValid(password, hash));
}

TEST_F(AuthControllerTest, UserWithToken_ConstructionAndToJson) {
    User user;
    user.setId(1);
    user.setUsername("testuser");

    AuthController::UserWithToken userWithToken(user);
    auto json = userWithToken.toJson();

    EXPECT_EQ(json["username"].asString(), "testuser");
    EXPECT_EQ(json["token"].asString(), "mocked_token");
}