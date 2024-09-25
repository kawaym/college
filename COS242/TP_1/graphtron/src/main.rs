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

    // let _ = measure_distance("./data");
    // let _ = create_tree("./data");
    // let _ = calculate_distance("./data", "20", "30");
    // println!("DFS: {:?}", dfs);

    let mut graph = parser::read_graph("./data/grafo_6.txt");
    graph.display_components_info();
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

fn create_tree(dir: &str) -> io::Result<()> {
    for entry in fs::read_dir(dir)? {
        let entry = entry?;
        let path = entry.path();

        if path.is_file() {
            if let Some(path_str) = path.to_str() {
                let mut graph = parser::read_graph(path_str);
                let start_time = Instant::now();
                let _ = graph.display_bfs("1");
                let duration = start_time.elapsed();

                println!("Árvore calculada em {}s", duration.as_secs_f64());
            }
        }
    }

    Ok(())
}

fn calculate_distance(dir: &str, start_id: &str, end_id: &str) -> io::Result<()> {
    for entry in fs::read_dir(dir)? {
        let entry = entry?;
        let path = entry.path();

        if path.is_file() {
            if let Some(path_str) = path.to_str() {
                let mut graph = parser::read_graph(path_str);
                let start_time = Instant::now();
                let _ = graph.display_distance(start_id, end_id);
                let duration = start_time.elapsed();

                println!(
                    "Distância calculada no grafo {} em {}s\n",
                    path_str,
                    duration.as_secs_f64()
                );
            }
        }
    }

    Ok(())
}
