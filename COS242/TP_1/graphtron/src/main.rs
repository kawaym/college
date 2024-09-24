use graphtron::parser;

fn main() {
    let mut graph = parser::read_graph("./data/grafo_1.txt");
    // graph.display("Lista".to_string());
    // let dfs = graph.dfs("1");
    // graph.display_dfs("1");
    // println!("------------------------------");
    // graph.display_bfs("1");
    graph.display_distance("4", "1");
    graph.calculate_diameter();
    // println!("DFS: {:?}", dfs);
}
