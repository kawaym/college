use std::fs;
use std::io;
use std::time::Instant;

use graphtron::parser;

fn main() {
    // let mut graph = parser::read_graph("./data/grafo_1.txt");
    // graph.display("Lista".to_string());
    // let dfs = graph.dfs("1");
    // graph.display_dfs("1");
    // println!("------------------------------");
    // graph.display_bfs("1");
    // graph.display_distance("4", "1");
    // graph.calculate_diameter();

    let _ = measure_distance("./data");
    // println!("DFS: {:?}", dfs);
}

fn measure_distance(dir: &str) -> io::Result<()> {
    for entry in fs::read_dir(dir)? {
        let entry = entry?;
        let path = entry.path();

        if path.is_file() {
            if let Some(path_str) = path.to_str() {
                let mut graph = parser::read_graph(path_str);
                let start_time = Instant::now();
                let diameter = graph.calculate_diameter();
                let duration = start_time.elapsed();

                println!(
                    "O diâmetro para o grafo do arquivo {} é de: {}. Esse dado foi calculado em: {}s",
                    path_str, diameter, duration.as_secs_f64()
                );
            }
        }
    }

    Ok(())
}
