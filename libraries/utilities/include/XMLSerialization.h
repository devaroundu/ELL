////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     XMLSerialization.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Format.h"

// types
#include "types.h"

// stl
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <type_traits>
#include <memory>

namespace utilities
{
    /// <summary> An XML serializer. </summary>
    class XMLSerializer
    {
    public:

        XMLSerializer(std::ostream& stream);

        /// <summary> Serialize fundamental types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being serialized. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The variable being serialized. </param>
        template<typename ValueType>
        void Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept = nullptr);

        /// <summary> Serialize a positive-size vector. </summary>
        ///
        /// <typeparam name="ElementType"> The type of vector elements being serialized. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The vector being serialized, must have size>0. </param>
        template<typename ElementType>
        void Serialize(const char* name, const std::vector<ElementType>& value);

        /// <summary> Serialize a shared pointer to a polymorphic class. </summary>
        ///
        /// <typeparam name="ValueType"> The type pointed to, must be a polymorphic class. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The variable being serialized. </param>
        template<typename ValueType>
        void Serialize(const char* name, const std::unique_ptr<ValueType>& value);

        /// <summary> Serialize class types. </summary>
        ///
        /// <typeparam name="ValueType"> Type being serialized. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The variable being serialized. </param>
        template<typename ValueType>
        void Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept = nullptr);

    private:
        template<typename ValueType>
        void WriteSingleLineTags(const std::string& tagName, const ValueType& value);

        template<typename ValueType>
        void WriteSingleLineTags(const std::string& tagName, const std::string& name, const ValueType& value);

        void WriteOpenTag(const std::string& tagName);

        template <typename ValueType>
        void WriteOpenTag(const std::string& tagName, const std::string& attributeName, const ValueType& attributeValue);

        template <typename ValueType1, typename ValueType2>
        void WriteOpenTag(const std::string& tagName, const std::string& attributeName1, const ValueType1& attributeValue1, const std::string& attributeName2, const ValueType2& attributeValue2);

        void WriteCloseTag(const std::string& tagName);

        void Indent();

        template<typename ValueType>
        void SerializeUnnamed(const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept = nullptr);

        template<typename ElementType>
        void SerializeUnnamed(const std::vector<ElementType>& value);

        template<typename ValueType>
        void SerializeUnnamed(const std::unique_ptr<ValueType>& value);

        template<typename ValueType>
        void SerializeUnnamed(const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept = nullptr);

        uint64 _indentation = 0;
        std::ostream& _stream;
    };

    /// <summary> An XML deserializer. </summary>
    class XMLDeserializer
    {
    public:

        /// <summary> Constructs a XMLDeserializer. </summary>
        ///
        /// <param name="iStream"> [in,out] The input stream. </param>
        XMLDeserializer(std::istream& stream);

        /// <summary> Deserialize fundamental types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename ValueType>
        void Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept = nullptr);

        /// <summary> Deserialize vector types. </summary>
        ///
        /// <typeparam name="ElementType"> The type of vector elements being deserialized. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename ElementType>
        void Deserialize(const char* name, std::vector<ElementType>& value);

        /// <summary> Deserialize a shared pointer to a polymorphic class. </summary>
        ///
        /// <typeparam name="ValueType"> The type pointed to, must be a polymorphic class. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename ValueType>
        void Deserialize(const char* name, std::unique_ptr<ValueType>& value);

        /// <summary> Deserialize class types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename ValueType>
        void Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept = nullptr);

    private:

        template <typename TagType>
        void ReadOpenTag(TagType&& tagName);

        template <typename TagType, typename NameType, typename AttributeType>
        void ReadOpenTag(TagType&& tagName, NameType&& attributeName, AttributeType&& attributeValue);

        template <typename TagType, typename NameType1, typename AttributeType1, typename NameType2, typename AttributeType2>
        void ReadOpenTag(TagType&& tagName, NameType1&& attributeName1, AttributeType1&& attributeValue1, NameType2&& attributeName2, AttributeType2&& attributeValue2);

        template <typename TagType>
        void ReadCloseTag(TagType&& tagName);

        template<typename TagType, typename ValueType>
        void ReadSingleLineTags(TagType&& tagName, ValueType&& value);

        template<typename TagType, typename NameType, typename AttributeType, typename ValueType>
        void ReadSingleLineTags(TagType&& tagName, NameType&& attributeName, AttributeType attributeValue, ValueType&& value);

        template<typename ValueType>
        void DeserializeUnnamed(ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept = nullptr);

        template<typename ElementType>
        void DeserializeUnnamed(std::vector<ElementType>& value);

        template<typename ValueType>
        void DeserializeUnnamed(std::unique_ptr<ValueType>& value);

        template<typename ValueType>
        void DeserializeUnnamed(ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept = nullptr);

        std::string _string;
        const char* _pStr;
    };
}

#include "../tcc/XMLSerialization.tcc"