use graphtron::parser;

fn main() {
    // parser::create_study_cases();
    // let graph = parser::read_graph("./data/grafo_W_2.txt");
    // graph.display_adjacency_list();
    // println!("{}", graph.has_negative_weight());
    // graph.display_dijkstra_heap("1");
    match parser::create_weighted_distances_study_case("grafo_W_1") {
        Ok(()) => (),
        Err(value) => println!("{value}"),
    }
}
