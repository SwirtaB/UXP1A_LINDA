#include "LindaTuple.hpp"

#include "Buffer.hpp"
#include "Debug.hpp"

#include <optional>
#include <stdexcept>
#include <string>
#include <variant>

namespace linda
{

Tuple::Builder::Builder() {}

Tuple::Builder &Tuple::Builder::String(const std::string &s) {
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
            throw std::runtime_error("Tuple::serialize - variant check not exhaustive");
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
            throw std::runtime_error("Tuple::deserialize - invalid TupleType");
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

TuplePattern::Builder &TuplePattern::Builder::stringOf(RequirementType rt, const std::string &s) {
    schema_.push_back(static_cast<char>(TupleType::String));
    requirements_.emplace_back(std::make_pair(static_cast<RequirementTypeSerializable>(rt), s));
    TupleValue t("asd");
    return *this;
}

TuplePattern::Builder &TuplePattern::Builder::anyInt() {
    schema_.push_back(static_cast<char>(TupleType::Int));
    requirements_.emplace_back(std::nullopt);
    return *this;
}

TuplePattern::Builder &TuplePattern::Builder::intOf(RequirementType rt, int i) {
    schema_.push_back(static_cast<char>(TupleType::Int));
    requirements_.emplace_back(std::make_pair(static_cast<RequirementTypeSerializable>(rt), i));
    return *this;
}

TuplePattern::Builder &TuplePattern::Builder::anyFloat() {
    schema_.push_back(static_cast<char>(TupleType::Float));
    requirements_.emplace_back(std::nullopt);
    return *this;
}

TuplePattern::Builder &TuplePattern::Builder::floatOf(RequirementType rt, float f) {
    schema_.push_back(static_cast<char>(TupleType::Float));
    requirements_.emplace_back(std::make_pair(static_cast<RequirementTypeSerializable>(rt), f));
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
        if (requirements_[i].has_value()) {
            auto &requirement = requirements_[i].value();
            if (requirement.first == RequirementTypeSerializable::Eq) {
                if (!(tuple.values()[i] == requirement.second)) {
                    return false;
                }
            } else if (requirement.first == RequirementTypeSerializable::Less) {
                if (!(tuple.values()[i] < requirement.second)) {
                    return false;
                }
            } else if (requirement.first == RequirementTypeSerializable::LessEq) {
                if (!(tuple.values()[i] <= requirement.second)) {
                    return false;
                }
            } else if (requirement.first == RequirementTypeSerializable::More) {
                if (!(tuple.values()[i] > requirement.second)) {
                    return false;
                }
            } else if (requirement.first == RequirementTypeSerializable::MoreEq) {
                if (!(tuple.values()[i] >= requirement.second)) {
                    return false;
                }
            } else {
                throw std::runtime_error("TuplePattern::matches - invalid requirement type");
            }
        }
    }
    return true;
}

std::vector<char> TuplePattern::serialize() {
    BufferEncoder encoder;
    encoder.pushString(schema_);
    for (TupleRequirement &requirement : requirements_) {
        if (requirement.has_value()) {
            encoder.pushChar(static_cast<char>(requirement.value().first));
            TupleValue value = requirement.value().second;
            if (std::holds_alternative<std::string>(value)) {
                encoder.pushString(std::get<std::string>(value));
            } else if (std::holds_alternative<int>(value)) {
                encoder.pushInt(std::get<int>(value));
            } else if (std::holds_alternative<float>(value)) {
                encoder.pushFloat(std::get<float>(value));
            } else {
                throw std::runtime_error("TuplePattern::serialize - variant check not exhaustive");
            }
        } else {
            encoder.pushChar(static_cast<char>(RequirementTypeSerializable::Any));
        }
    }
    return encoder.encode();
}

TuplePattern TuplePattern::deserialize(const std::vector<char> &data) {
    BufferDecoder         decoder = BufferDecoder::decode(data);
    TuplePattern::Builder builder;
    std::string           schema = decoder.readString();
    for (char c : schema) {
        TupleType                   type             = static_cast<TupleType>(c);
        RequirementTypeSerializable requirement_type = static_cast<RequirementTypeSerializable>(decoder.readChar());
        if (type == TupleType::String) {
            if (requirement_type != RequirementTypeSerializable::Any) {
                builder.stringOf(static_cast<RequirementType>(requirement_type), decoder.readString());
            } else {
                builder.anyString();
            }
        } else if (type == TupleType::Int) {
            if (requirement_type != RequirementTypeSerializable::Any) {
                builder.intOf(static_cast<RequirementType>(requirement_type), decoder.readInt());
            } else {
                builder.anyInt();
            }
        } else if (type == TupleType::Float) {
            if (requirement_type != RequirementTypeSerializable::Any) {
                builder.floatOf(static_cast<RequirementType>(requirement_type), decoder.readFloat());
            } else {
                builder.anyFloat();
            }
        } else {
            throw std::runtime_error("TuplePattern::deserialize - invalid TupleType");
        }
    }
    return builder.build();
}

} // namespace linda