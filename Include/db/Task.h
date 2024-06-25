#ifndef APP_DB_TASK_H
#define	APP_DB_TASK_H

#include "Config.h"
#include <string>
#include <vector>
#include "db/Util.h"

namespace app {
namespace db {
class Connector;
class Task;
using FuncDBTask = void (*)(Task*, Connector*);

/**
 * @brief 灏佽