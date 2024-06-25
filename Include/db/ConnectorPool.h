#ifndef APP_DB_CONNECTORPOOL_H
#define	APP_DB_CONNECTORPOOL_H


#include "db/ConnectConfig.h"
#include "db/Connector.h"
#include "db/ConnectorHolder.h"
#include <atomic>
#include <mutex>

namespace app {
namespace db {

class Database;


/**
* @brief 鏁版嵁搴撹繛鎺ユ睜
*/
class ConnectorPool {
    friend Database;
    friend ConnectorHolder;

public:
    ConnectorPool() = delete;
    ~ConnectorPool();

    ConnectorPool(const ConnectorPool&) = delete;
    ConnectorPool(ConnectorPool&&) = delete;
    ConnectorPool& operator=(const ConnectorPool&) noexcept = delete;
    ConnectorPool& operator=(ConnectorPool&&) noexcept = delete;

    const ConnectConfig& getConnectionInfo() const {
        return mConInfo;
    }


private:
    std::mutex mMutex;
    Connector* mConQueue;
    ConnectConfig mConInfo;

    explicit ConnectorPool(ConnectConfig connection);


    /**
     * @brief pop a Connector from the pool with the provided task, timeout and on complete callback.
     * @return A Connector handle.
     */
    Connector* pop();


    /**
    * @brief 灏嗚繛鎺ヨ繕鍥炴暟鎹