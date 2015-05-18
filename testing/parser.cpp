#include <iostream>
#include <memory>
#include "../slottedpages/Parser.h"
#include "../slottedpages/segment/Schema.h"
#include "../buffer/BufferManager.h"
#include "../slottedpages/segment/SchemaIO.h"

bool testEqual(Schema a, Schema b){
    return a.toString() == b.toString();
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <schema file>" << std::endl;
        return -1;
    }

    Parser p(argv[1]);
    try {
        std::unique_ptr<Schema> schema = p.parse();
        std::cout << schema->toString() << std::endl;
    } catch (ParserError &e) {
        std::cerr << e.what() << std::endl;
    }

    Schema schema1 = *p.parse().get();

    BufferManager bm(100);
    SchemaIO schemaIO(bm);

    schemaIO.save(schema1);
    Schema schema2 = schemaIO.load();

    if(testEqual(schema1, schema2)){
        std::cout << "Schemas are identical." << std::endl;
    } else std::cout << "Schemas differ!" << std::endl;
    return 0;
}
