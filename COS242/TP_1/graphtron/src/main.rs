mod graph;
mod parser;

fn main() {
    let mut graph = parser::read_graph("./data/grafo_teste.txt");
    graph.display();
    let number = graph.get_edges_number();
    println!("{}", number)
}
