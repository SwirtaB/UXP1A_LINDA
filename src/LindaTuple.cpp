#include "LindaTuple.hpp"

#include "Buffer.hpp"

#include <optional>
#include <string>
#include <variant>

namespace linda
{

Tuple::Builder::Builder() {}

Tuple::Builder &Tuple::Builder::String(std::string &&s) {
    schema_.push_back(static_cast<char>(TupleType::String));
    values_.emplace_back(s);
    return *this;
}

Tuple::Builder &Tuple::Builder::Int(int i) {
    schema_.push_back(static_cast<char>(TupleType::Int));
    values_.emplace_back(i);
    return *this;
}

Tuple::Builder &Tuple::Builder::Float(float f) {
    schema_.push_back(static_cast<char>(TupleType::Float));
    values_.emplace_back(f);
    return *this;
}

Tuple Tuple::Builder::build() {
    Tuple t;
    t.schema_ = schema_;
    t.values_ = values_;
    return t;
}

const std::string &Tuple::schema() {
    return schema_;
}

const std::vector<TupleValue> &Tuple::values() {
    return values_;
}

std::vector<char> Tuple::serialize() {
    BufferEncoder encoder;
    encoder.pushString(schema_);
    for (auto &value : values_) {
        if (std::holds_alternative<std::string>(value)) {
            encoder.pushString(std::get<std::string>(value));
        } else if (std::holds_alternative<int>(value)) {
            encoder.pushInt(std::get<int>(value));
        } else if (std::holds_alternative<float>(value)) {
            encoder.pushFloat(std::get<float>(value));
        } else {
            throw "unreachable";
        }
    }
    return encoder.encode();
}

Tuple Tuple::deserialize(const std::vector<char> &data) {
    BufferDecoder  decoder = BufferDecoder::decode(data);
    Tuple::Builder builder;
    std::string    schema = decoder.readString();
    for (char c : schema) {
        TupleType type = static_cast<TupleType>(c);
        if (type == TupleType::String) {
            builder.String(decoder.readString());
        } else if (type == TupleType::Int) {
            builder.Int(decoder.readInt());
        } else if (type == TupleType::Float) {
            builder.Float(decoder.readFloat());
        } else {
            throw "unreachable";
        }
    }
    return builder.build();
}

TuplePattern::Builder::Builder() {}

TuplePattern::Builder &TuplePattern::Builder::anyString() {
    schema_.push_back(static_cast<char>(TupleType::String));
    requirements_.emplace_back(std::nullopt);
    return *this;
}

TuplePattern::Builder &TuplePattern::Builder::stringOf(std::string &&s) {
    schema_.push_back(static_cast<char>(TupleType::String));
    requirements_.emplace_back(s);
    TupleValue t("asd");
    return *this;
}

TuplePattern::Builder &TuplePattern::Builder::anyInt() {
    schema_.push_back(static_cast<char>(TupleType::Int));
    requirements_.emplace_back(std::nullopt);
    return *this;
}

TuplePattern::Builder &TuplePattern::Builder::intOf(int i) {
    schema_.push_back(static_cast<char>(TupleType::String));
    requirements_.emplace_back(i);
    return *this;
}

TuplePattern::Builder &TuplePattern::Builder::anyFloat() {
    schema_.push_back(static_cast<char>(TupleType::Float));
    requirements_.emplace_back(std::nullopt);
    return *this;
}

TuplePattern::Builder &TuplePattern::Builder::floatOf(float f) {
    schema_.push_back(static_cast<char>(TupleType::String));
    requirements_.emplace_back(f);
    return *this;
}

TuplePattern TuplePattern::Builder::build() {
    TuplePattern tp;
    tp.schema_       = schema_;
    tp.requirements_ = requirements_;
    return tp;
}

const std::string &TuplePattern::schema() {
    return schema_;
}

bool TuplePattern::matches(Tuple &tuple) {
    if (schema_ != tuple.schema()) {
        return false;
    }
    for (int i = 0; i < schema_.size(); ++i) {
        if (requirements_[i].has_value() && requirements_[i].value() != tuple.values()[i]) {
            return false;
        }
    }
    return true;
}

std::vector<char> TuplePattern::serialize() {
    BufferEncoder encoder;
    encoder.pushString(schema_);
    for (TupleRequirement &requirement : requirements_) {
        if (requirement.has_value()) {
            encoder.pushChar(1);
            TupleValue value = requirement.value();
            if (std::holds_alternative<std::string>(value)) {
                encoder.pushString(std::get<std::string>(value));
            } else if (std::holds_alternative<int>(value)) {
                encoder.pushInt(std::get<int>(value));
            } else if (std::holds_alternative<float>(value)) {
                encoder.pushFloat(std::get<float>(value));
            } else {
                throw "unreachable";
            }
        } else {
            encoder.pushChar(0);
        }
    }
    return encoder.encode();
}

TuplePattern TuplePattern::deserialize(std::vector<char> &data) {
    BufferDecoder         decoder = BufferDecoder::decode(data);
    TuplePattern::Builder builder;
    std::string           schema = decoder.readString();
    for (char c : schema) {
        TupleType type      = static_cast<TupleType>(c);
        char      has_value = decoder.readChar();
        if (type == TupleType::String) {
            if (has_value) {
                builder.stringOf(decoder.readString());
            } else {
                builder.anyString();
            }
        } else if (type == TupleType::Int) {
            if (has_value) {
                builder.intOf(decoder.readInt());
            } else {
                builder.anyInt();
            }
        } else if (type == TupleType::Float) {
            if (has_value) {
                builder.floatOf(decoder.readFloat());
            } else {
                builder.anyFloat();
            }
        } else {
            throw "unreachable";
        }
    }
    return builder.build();
}

} // namespace linda