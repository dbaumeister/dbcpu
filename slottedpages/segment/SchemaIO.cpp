//
// Created by dbaumeister on 16.05.15.
//

#include <iostream>
#include <assert.h>
#include "SchemaIO.h"

void SchemaIO::save(Schema schema) {
    BufferFrame* bufferFrame = bufferManager.fixPage(0, true);
    char* data = (char*)bufferFrame->getData();

    append(schema, data, 0); //only save one schema

    bufferManager.unfixPage(bufferFrame, true);
}

uint16_t SchemaIO::append(Schema schema, char* data, uint16_t offset) {
    //save <ptrRelation1End> <Relation1> <ptrRelation2End> <Relation2>
    uint16_t len = 0;
    for(int i = 0; i < schema.relations.size(); ++i) {
        //save for each relation
        Schema::Relation relation = schema.relations[i];
        len += append(relation, data, offset + len + (uint16_t) sizeof(uint16_t));
    }
    memcpy(data + offset, &len, sizeof(uint16_t));
    return len + (uint16_t)sizeof(uint16_t);
}

uint16_t SchemaIO::append(Schema::Relation relation, char* data, uint16_t offset) {
    // <NameEnd> <Name> <NumPrimKeys> <PrimKey1> <PrimKey2> ... <ptrAttr1End> <Attr1> <ptrAttr2End> <ptrAttr2>
    uint16_t len = 0;
    uint16_t nameLen = (uint16_t) relation.name.size();
    memcpy(data + offset + len + sizeof(uint16_t), &nameLen, sizeof(uint16_t));
    len += sizeof(uint16_t);

    const char* name = relation.name.c_str();
    memcpy(data + offset + len + sizeof(uint16_t), name, nameLen);
    len += nameLen;

    uint16_t numPrimKeys = (uint16_t) relation.primaryKey.size();
    memcpy(data + offset + len + sizeof(uint16_t), &numPrimKeys, sizeof(uint16_t));
    len += sizeof(uint16_t);


    for(auto it : relation.primaryKey){
        memcpy(data + offset + len + sizeof(uint16_t), &it, sizeof(unsigned int));
        len += sizeof(unsigned int);
    }

    for(int i = 0; i < relation.attributes.size(); ++i){
        //save for each attribute
        Schema::Relation::Attribute attribute = relation.attributes[i];
        len += append(attribute, data, offset + len + (uint16_t) sizeof(uint16_t));

    }
    memcpy(data + offset, &len, sizeof(uint16_t));

    std::cout << "Relation has length: " << len << std::endl;
    return len + (uint16_t)sizeof(uint16_t);
}

uint16_t SchemaIO::append(Schema::Relation::Attribute attribute, char *data, uint16_t offset) {
    uint16_t len = 0;

    memcpy(data + offset, &attribute.len, sizeof(unsigned int));
    len += (uint16_t)sizeof(unsigned int);

    memcpy(data + offset + len, &attribute.notNull, sizeof(bool));
    len += (uint16_t)sizeof(bool);

    //save type as integer
    uint16_t type = 0; //Integer
    if(attribute.type == Types::Tag::Char){
        type = 1;
    }
    memcpy(data + offset + len, &type, sizeof(uint16_t));
    len += (uint16_t) sizeof(uint16_t);

    uint16_t nameLen = (uint16_t) attribute.name.size();
    const char* name = attribute.name.c_str();

    memcpy(data + offset + len, &nameLen, sizeof(uint16_t));
    len += (uint16_t)sizeof(uint16_t);

    memcpy(data + offset + len, &name, nameLen);
    len += nameLen;
    return len;
}

Schema SchemaIO::load() {

    BufferFrame* bufferFrame = bufferManager.fixPage(0, true);
    char* data = (char*)bufferFrame->getData();

    uint16_t len = 0;
    memcpy(&len, data, sizeof(uint16_t));

    Schema schema = getSchema(data, sizeof(uint16_t), len);

    bufferManager.unfixPage(bufferFrame, false);
    return schema;
}

Schema SchemaIO::getSchema(char *data, uint16_t offset, uint16_t len) {
    uint16_t loffset = offset;

    Schema schema;
    schema.relations = std::vector<Schema::Relation>();

    while(loffset < len + offset){

        uint16_t rlen = 0;
        memcpy(&rlen, data + loffset, sizeof(uint16_t)); //read length of next relation into rlen
        std::cout << "rlen: " << rlen << std::endl;
        loffset += sizeof(uint16_t);

        schema.relations.push_back(getRelation(data, loffset, rlen));
        loffset += rlen;
    }
    assert(loffset == len + offset);
    return schema;
}

Schema::Relation SchemaIO::getRelation(char *data, uint16_t offset, uint16_t len) {

    uint16_t loffset = offset;

    //TODO relation params
    std::string name;

    Schema::Relation relation(name);
    relation.attributes = std::vector<Schema::Relation::Attribute>();


    return relation;

}
