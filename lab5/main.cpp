#include <filesystem>
#include <fstream>
#include <iostream>
#include <common/cmdline_positional_parser.h>
#include <common/stream-extension.h>
#include <lab5/entity_relationship_diagram.h>
#include <gvc.h>
#include <gvplugin.h>

void setup_input_stream(std::ifstream& fin, istream_extension& in, const std::string& source) {
    if (source != "stdin") {
        if (std::filesystem::exists(source)) {
            fin.open(source);
            in.rdbuf(fin.rdbuf());
        } else {
            throw std::runtime_error(std::string("File wasn't found at given filepath: ") + source);
        }
    }
}

int main(int argc, const char* argv[]){
    PositionalOptionsDescription description("Allowed options:");
    description.add_options(ParserOption("Grammar source ::= [\"filepath\" | stdin] ", "stdin"));
    CmdlinePositionalParser parser(argc, argv, description);
    istream_extension input(std::cin.rdbuf());
    std::ifstream finput;
    setup_input_stream(finput, input, parser[1].as<std::string>());
    EntityRelationshipDiagram diagram;
    input >> diagram;
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y-%H-%M-%S");
    std::string ER_filename = "ER_model-" + oss.str() + ".svg";
    std::string Rel_filename = "Relational_model-" + oss.str() + ".svg";
    std::string csv_table_filename = "Cardinalities_table_of_Rel_model-" + oss.str() + ".csv";
    std::filesystem::path ER_path = argv[0], Rel_path = argv[0], Table_path = argv[0];
    ER_path.replace_filename(ER_filename);
    Rel_path.replace_filename(Rel_filename);
    Table_path.replace_filename(csv_table_filename);
    diagram.save_svg(ER_path);
    diagram.to_relational_model().save_svg(Rel_path);
    diagram.save_cardinalities_table_to_csv(Table_path);
    return 0;
}