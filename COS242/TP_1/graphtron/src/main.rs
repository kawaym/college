mod graph;
mod parser;

fn main() {
    let mut graph = parser::read_graph("./data/grafo_teste.txt");
    let number = graph.get_degree_median();
    println!("{}", number);
    graph.display_adjacency_matrix();
}
