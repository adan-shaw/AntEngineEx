#ifndef APP_DB_VALUE_H
#define	APP_DB_VALUE_H

#include "Converter.h"
#include "Strings.h"

namespace app {
namespace db {

class Row;

class Value {
    friend Row;

public:
    Value() :mValid(false) { }
    ~Value() = default;
    Value(const Value&) = default;
    Value(Value&&) = default;
    Value& operator=(const Value&) = default;
    Value& operator=(Value&&) = default;


    bool isValid() const;

    /**
     * @brief 杞