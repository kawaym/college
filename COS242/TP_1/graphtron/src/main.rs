mod graph;
mod parser;

fn main() {
    let mut graph = parser::read_graph("./data/grafo_teste.txt");
    graph.display("Matriz".to_string());
    let dfs = graph.dfs("1");
    let bfs = graph.bfs("1");

    println!("DFS: {:?}", dfs);
    println!("BFS: {:?}", bfs);
}
