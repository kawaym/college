use graphtron::Graph;

fn main() {
    let mut graph = Graph::new();
    graph.add_vertex("1".to_string());
    graph.display();
    println!("Hello, world!");
}
