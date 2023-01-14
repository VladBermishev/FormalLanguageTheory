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
    std::filesystem::path ER_path = argv[0], Rel_path = argv[0];
    ER_path.replace_filename(ER_filename);
    Rel_path.replace_filename(Rel_filename);
    diagram.save_svg(ER_path);
    diagram.to_relational_model().save_svg(Rel_path);
    return 0;
}

/*
    Agraph_t *g;
    Agnode_t *n, *m;
    Agedge_t *e;
    Agsym_t *a;
    GVC_t *gvc;
    gvc = gvContext();
    g = agopen("g", Agdirected, nullptr);
    n = agnode(g, "n", 1);
    m = agnode(g, "m", 1);
    e = agedge(g, n, m, 0, 1);
    agsafeset(n, "color", "red", "");
    gvLayout(gvc, g, "dot");
    gvRenderFilename(gvc, g, "svg", "example.svg");
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc)
*/