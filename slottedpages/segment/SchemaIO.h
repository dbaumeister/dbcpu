//
// Created by dbaumeister on 16.05.15.
//

#ifndef PROJECT_SCHEMAIO_H
#define PROJECT_SCHEMAIO_H

#include "Schema.h"
#include "../../buffer/BufferManager.h"

class SchemaIO {
public:
    SchemaIO(BufferManager& bufferManager) : bufferManager(bufferManager) {}

    void save(Schema schema);
    Schema load();

private:
    BufferManager& bufferManager;

    uint16_t append(Schema schema, char* data, uint16_t offset);
    uint16_t append(Schema::Relation relation, char* data, uint16_t offset);
    uint16_t append(Schema::Relation::Attribute attribute, char* data, uint16_t offset);

    Schema getSchema(char* data, uint16_t offset, uint16_t len);
    Schema::Relation getRelation(char* data, uint16_t offset, uint16_t len);

};

#endif //PROJECT_SCHEMAIO_H
