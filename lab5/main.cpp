#include <filesystem>
#include <fstream>
#include <iostream>
#include <common/cmdline_positional_parser.h>
#include <common/stream-extension.h>
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
    return (gvFreeContext(gvc));
}