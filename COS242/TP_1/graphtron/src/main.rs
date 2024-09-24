mod graph;
mod parser;

fn main() {
    let mut graph = parser::read_graph("./data/grafo_teste.txt");
    // graph.display("Lista".to_string());
    // let dfs = graph.dfs("1");
    graph.display_dfs("1");
    println!("------------------------------");
    graph.display_bfs("1");

    // println!("DFS: {:?}", dfs);
}
